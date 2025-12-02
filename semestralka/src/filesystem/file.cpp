#include "errors.hpp"
#include "filesystem.hpp"
#include "structures.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <vector>

namespace jkfs {

void Filesystem::file_create(int32_t parent_id, std::string file_name) {
  int32_t id = -1;
  int32_t cluster = -1;
  bool parent_entry_added = false;

  try {
    id = inode_alloc();
    if (id < 0) {
      throw jkfilesystem_error("There is no empty inode.");
    }
    cluster = cluster_alloc();
    if (cluster < 0) {
      throw jkfilesystem_error("There is no empty cluster.");
    }

    // fill inode
    struct inode file{};
    file.node_id = id;
    file.is_dir = false;
    file.file_size = 0;
    file.direct[0] = cluster;

    // write inode
    inode_write(id, file);

    // add this to parent dir
    dir_item_add(parent_id, id, file_name);
    parent_entry_added = true;
  } catch (...) {
    // rollback
    if (parent_entry_added) {
      dir_item_remove(parent_id, file_name);
    }
    if (cluster >= 0) {
      cluster_free(cluster);
    }
    if (id >= 0) {
      inode_free(id);
    }

    // let others know the exception
    throw;
  }
}

void Filesystem::file_resize(int32_t inode_id, int32_t new_size) {
  auto inode = inode_read(inode_id);
  if (new_size < inode.file_size) {
    throw jkfilesystem_error("file_resize() cannot make the file smaller");
  } else if (new_size == inode.file_size) {
    return; // work already done
  }

  // integer ceiling
  int clusters_needed = (new_size + cluster_size_ - 1) / cluster_size_;
  auto allocated_clusters = file_list_clusters(inode_id);

  if (static_cast<size_t>(clusters_needed) <= allocated_clusters.size()) {
    return; // work already done
  }

  // allocate new clusters
  while (static_cast<size_t>(clusters_needed) > allocated_clusters.size()) {
    allocated_clusters.push_back(cluster_alloc());
  }

  // vector can only pop back, so reverse the order and have first cluster at
  // the end
  std::reverse(allocated_clusters.begin(), allocated_clusters.end());

  // direct
  for (int i = 0; i < static_cast<int>(std::size(inode.direct)); i++) {
    inode.direct[i] = allocated_clusters.back();
    allocated_clusters.pop_back();
  }

  // ensure indirect1 exists
  if (inode.indirect1 <= 0) {
    inode.indirect1 = cluster_alloc();
    if (inode.indirect1 <= 0) {
      throw jkfilesystem_error("Not enough empty clusters (for indirect1).");
    }
  }
  file_resize_cluster_indirect(inode.indirect1, allocated_clusters);

  // ensure indirect2 exists
  if (inode.indirect2 <= 0) {
    inode.indirect2 = cluster_alloc();
    if (inode.indirect2 <= 0) {
      throw jkfilesystem_error("Not enough empty clusters (for indirect2).");
    }
  }
  // read the indirect2 cluster
  auto indirect2s_bytes = cluster_read(inode.indirect2);
  std::span<int32_t> indirect2s{
      reinterpret_cast<int32_t *>(indirect2s_bytes.data()),
      indirect2s_bytes.size() / sizeof(int32_t)};

  // for each entry in indirect2 do indirect1
  size_t max_entries = static_cast<size_t>(cluster_size_) / sizeof(int32_t);
  for (size_t i = 0; i < max_entries; i++) {
    if (indirect2s[i] <= 0) {
      if (allocated_clusters.empty() <= 0) {
        break;
      }
      indirect2s[i] = cluster_alloc();
      if (indirect2s[i] <= 0) {
        throw jkfilesystem_error(
            "Not enough empty clusters (for indirect1 in indirect2).");
      }

      file_resize_cluster_indirect(indirect2s[i], allocated_clusters);
    }
  }

  // write back to cluster
  cluster_write(inode.indirect2,
                reinterpret_cast<const char *>(indirect2s_bytes.data()),
                cluster_size_);
}

void Filesystem::file_write(int32_t inode_id, int32_t offset, const char *data,
                            size_t data_size) {
  // list all clusters to write into
  auto clusters = file_list_clusters(inode_id);

  // if dont have enough space, resize
  if (static_cast<int>(clusters.size()) * cluster_size_ <
      static_cast<int>(data_size) + offset) {
    file_resize(inode_id, static_cast<int32_t>(data_size) + offset);
  }
  // reload clusters
  clusters = file_list_clusters(inode_id);

  size_t start_cluster_idx = static_cast<size_t>(offset / cluster_size_);
  size_t start_cluster_offset = static_cast<size_t>(offset % cluster_size_);

  // only for readability - static type here and not on X places below
  size_t cluster_size = static_cast<size_t>(cluster_size_);

  // look at the data differently
  std::span<const uint8_t> data_to_write(
      reinterpret_cast<const uint8_t *>(data), data_size);

  size_t written_bytes = 0;
  size_t written_clusters = 0;

  // work with the first cluster - different, because we need to write only
  // after offset
  auto first_cluster_contents = cluster_read(clusters[start_cluster_idx]);
  for (size_t i = start_cluster_offset; i < cluster_size; i++) {
    first_cluster_contents[i] = data_to_write[written_bytes];
    written_bytes++;
  }
  cluster_write(clusters[start_cluster_idx],
                reinterpret_cast<const char *>(first_cluster_contents.data()),
                cluster_size_);
  written_clusters++;

  // next clusters - write from cluster beginning
  while (written_bytes < data_size) {
    std::vector<uint8_t> contents(cluster_size);

    for (size_t i = 0; i < cluster_size; i++) {
      if (written_bytes >= data_size) {
        break; // already wrote everything
      }
      contents[i] = data_to_write[written_bytes];
      written_bytes++;
    }

    cluster_write(clusters[start_cluster_idx + written_clusters],
                  reinterpret_cast<const char *>(contents.data()),
                  cluster_size_);
    written_clusters++;
  }
}

std::vector<uint8_t> Filesystem::file_read(int32_t inode_id) {
  std::vector<uint8_t> file_contents;
  auto clusters = file_list_clusters(inode_id);

  for (const auto &cluster : clusters) {
    auto data = cluster_read(cluster);
    file_contents.insert(file_contents.end(), data.begin(), data.end());
  }

  return file_contents;
}

void Filesystem::file_delete(int32_t parent_inode_id, std::string file_name) {
  // find inode to delete
  auto inode = dir_lookup(parent_inode_id, file_name);

  // free clusters
  auto clusters = file_list_clusters(inode);
  for (const auto &cluster : clusters) {
    cluster_free(cluster);
  }

  // free inode
  inode_free(inode);
  // free dir item reference
  dir_item_remove(parent_inode_id, file_name);
}

// PRIVATE

std::vector<int32_t> Filesystem::file_list_clusters(int32_t inode_id) {
  std::vector<int32_t> clusters;
  auto inode = inode_read(inode_id);

  // direct
  for (const auto &idx : inode.direct) {
    if (idx <= 0) {
      return clusters;
    }
    clusters.push_back(idx);
  }

  // indirect 1
  if (inode.indirect1 <= 0) {
    return clusters;
  }
  auto indirect1 = file_list_clusters_indirect(inode.indirect1);
  clusters.insert(clusters.end(), indirect1.begin(), indirect1.end());

  // indirect 2
  if (inode.indirect2 <= 0) {
    return clusters;
  }
  auto indirects = file_list_clusters_indirect(inode.indirect2);
  for (const auto &indirect : indirects) {
    auto indirect2 = file_list_clusters_indirect(indirect);
    clusters.insert(clusters.end(), indirect2.begin(), indirect2.end());
  }

  return clusters;
}

std::vector<int32_t>
Filesystem::file_list_clusters_indirect(int32_t cluster_idx) {
  std::vector<int32_t> clusters;
  auto bytes = cluster_read(cluster_idx);
  std::span<const int32_t> cluster_idxs{
      reinterpret_cast<const int32_t *>(bytes.data()),
      bytes.size() / sizeof(int32_t)};

  for (const auto &idx : cluster_idxs) {
    if (idx <= 0) {
      return clusters;
    }
    clusters.push_back(idx);
  }

  return clusters;
}

void Filesystem::file_resize_cluster_indirect(int32_t indirect_id,
                                              std::vector<int32_t> &to_write) {
  // read bytes in indirect cluster
  auto indirect_bytes = cluster_read(indirect_id);
  // look at them as if they are cluster indexes
  auto indirect_entries = reinterpret_cast<int32_t *>(indirect_bytes.data());

  int32_t max_entries = cluster_size_ / static_cast<int>(sizeof(int32_t));
  for (int i = 0; i < max_entries; i++) {
    if (indirect_entries[i] <= 0) { // empty entry - only >0 are valid indexes
      if (to_write.empty()) {       // no more clusters to add
        break;
      }

      // fill with new cluster id
      indirect_entries[i] = to_write.back();
      to_write.pop_back();
    }
  }

  cluster_write(indirect_id,
                reinterpret_cast<const char *>(indirect_bytes.data()),
                cluster_size_);
}

} // namespace jkfs
