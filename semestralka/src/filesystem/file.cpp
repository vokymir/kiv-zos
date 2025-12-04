#include "errors.hpp"
#include "filesystem.hpp"
#include "structures.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <vector>

namespace jkfs {

int32_t Filesystem::file_create(int32_t parent_id, std::string file_name) {
  int32_t id = -1;

  try {
    id = inode_alloc();
    if (id < 0) {
      throw jkfilesystem_error("There is no empty inode for file.");
    }

    // fill inode
    struct inode file{};
    file.node_id = id;
    file.is_dir = false;
    file.file_size = 0;

    // write inode
    inode_write(id, file);

    // add this to parent dir
    dir_item_add(parent_id, id, file_name);

    // return inode id
    return id;
  } catch (...) {
    // rollback
    file_delete(parent_id, file_name);

    // let others know the exception
    throw;
  }
}

void Filesystem::file_resize(int32_t inode_id, int32_t new_size) {
  auto inode = inode_read(inode_id);
  if (new_size < inode.file_size) {
    return; // work already done
  } else if (new_size == inode.file_size) {
    return; // work already done
  }

  auto all_clusters = file_resize_allocate_clusters(inode_id, new_size);
  // for atomicity
  std::vector<int32_t> copy_all_clusters(all_clusters.size());
  std::copy(all_clusters.begin(), all_clusters.end(),
            copy_all_clusters.begin());

  // vector can only pop back, so reverse the order and have first cluster at
  // the end
  std::reverse(all_clusters.begin(), all_clusters.end());
  // strategy: rewrite all existing values, because apart from time it is not
  // against anything :D

  try {
    // direct
    for (int i = 0; i < static_cast<int>(std::size(inode.direct)); i++) {
      if (all_clusters.empty()) {
        break;
      }
      inode.direct[i] = all_clusters.back();
      all_clusters.pop_back();
      if (all_clusters.empty()) {
        goto end;
      }
    }

    // indirect1
    if (inode.indirect1 <= 0) {
      inode.indirect1 = cluster_alloc();
      if (inode.indirect1 <= 0) {
        throw jkfilesystem_error("Not enough empty clusters (for indirect1).");
      }
    }
    file_resize_cluster_indirect1(inode.indirect1, all_clusters);

    if (all_clusters.empty()) {
      goto end;
    }

    // indirect2
    if (inode.indirect2 <= 0) {
      inode.indirect2 = cluster_alloc();
      if (inode.indirect2 <= 0) {
        throw jkfilesystem_error("Not enough empty clusters (for indirect2).");
      }
    }
    file_resize_cluster_indirect2(inode.indirect2, all_clusters);

  end:
    // write (potentially) changed inode
    inode.file_size = new_size;
    inode_write(inode_id, inode);
  } catch (...) {
    // TODO: not anymore
    // UPDATE: probably doesn't matter for semestral work
    // atomicity strategy:
    // 1. direct fail - OK
    // 2. indirect1 fail - OK, is atomic & direct need no reverting
    // 3. indirect2 fail - its atomic, but need to revert indirect1: PANIC
    // 4. inode_write fail - PANIC

    throw;
  }
}

void Filesystem::file_write(int32_t inode_id, int32_t offset, const char *data,
                            size_t data_size) {
  // if dont have enough space, resize
  file_resize(inode_id, static_cast<int32_t>(data_size) + offset);

  // list all clusters to write into
  auto clusters = file_list_clusters(inode_id);

  size_t start_cluster_idx = static_cast<size_t>(offset / cluster_size_);
  size_t start_cluster_offset = static_cast<size_t>(offset % cluster_size_);

  // look at the data differently
  std::span<uint8_t> data_to_write(
      reinterpret_cast<uint8_t *>(const_cast<char *>(data)), data_size);

  size_t written_bytes = 0;

  while (written_bytes < data_size) {
    // get current cluster index
    auto cluster = clusters[start_cluster_idx + written_bytes / cluster_size_];
    // if 1st cluster: give offset, otherwise 0
    auto offset = written_bytes < cluster_size_ ? written_bytes : 0;

    file_write_cluster(cluster, offset, data_to_write, written_bytes);
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
      return clusters; // already have all
    }
    clusters.push_back(idx);
  }

  // indirect 1
  if (inode.indirect1 <= 0) {
    return clusters; // already have all
  }
  auto indirect1 = file_list_clusters_indirect(inode.indirect1);
  clusters.insert(clusters.end(), indirect1.begin(), indirect1.end());

  // indirect 2
  if (inode.indirect2 <= 0) {
    return clusters; // already have all
  }
  // load where to look
  auto indirects = file_list_clusters_indirect(inode.indirect2);
  // look there
  for (const auto &indirect : indirects) {
    auto indirect2 = file_list_clusters_indirect(indirect);
    if (indirect2.empty()) {
      return clusters; // already have all
    }
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

std::vector<int32_t>
Filesystem::file_resize_allocate_clusters(int32_t inode_id, int32_t new_size) {
  // integer ceiling
  int clusters_needed = (new_size + cluster_size_ - 1) / cluster_size_;
  auto allocated_clusters = file_list_clusters(inode_id);
  // for atomicity - used in try/catch
  auto original_size = allocated_clusters.size();

  if (static_cast<size_t>(clusters_needed) <= allocated_clusters.size()) {
    return allocated_clusters; // work already done
  }

  try {
    // allocate new clusters
    while (static_cast<size_t>(clusters_needed) > allocated_clusters.size()) {
      allocated_clusters.push_back(cluster_alloc());
    }
  } catch (...) {
    // free all newly allocated clusters
    while (allocated_clusters.size() > original_size) {
      cluster_free(allocated_clusters.back());
      allocated_clusters.pop_back();
    }

    // let others know error happened
    throw;
  }

  return allocated_clusters;
}

void Filesystem::file_resize_cluster_indirect1(int32_t indirect_id,
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

void Filesystem::file_resize_cluster_indirect2(
    int32_t cluster_idx, std::vector<int32_t> &to_write_from_back) {
  // read the indirect2 cluster
  auto bytes = cluster_read(cluster_idx);
  std::span<int32_t> indirect2s{reinterpret_cast<int32_t *>(bytes.data()),
                                bytes.size() / sizeof(int32_t)};

  // for atomicity
  std::vector<int32_t> allocated_here;

  try {
    // for each entry in indirect2 do indirect1
    size_t max_entries = static_cast<size_t>(cluster_size_) / sizeof(int32_t);
    for (size_t i = 0; i < max_entries; i++) {
      if (indirect2s[i] <= 0) { // if entry in indirect2 is invalid
        if (to_write_from_back.empty() <= 0) {
          break;
        }
        // allocate new cluster
        indirect2s[i] = cluster_alloc();
        if (indirect2s[i] <= 0) {
          throw jkfilesystem_error(
              "Not enough empty clusters (for indirect1 in indirect2).");
        }
        allocated_here.push_back(indirect2s[i]); // atomicity

        file_resize_cluster_indirect1(indirect2s[i], to_write_from_back);
      }
    }
  } catch (...) {
    for (const auto &cluster : allocated_here) {
      cluster_free(cluster);
    }

    // let others know
    throw;
  }

  // write back to cluster
  cluster_write(cluster_idx, reinterpret_cast<const char *>(bytes.data()),
                cluster_size_);
}

void Filesystem::file_write_cluster(int32_t cluster_idx,
                                    int32_t offset_in_cluster,
                                    const std::span<uint8_t> &data_to_write,
                                    size_t &written_bytes) {
  std::vector<uint8_t> raw;
  if (offset_in_cluster > 0) {
    raw = cluster_read(cluster_idx);
  } else {
    raw.resize(cluster_size_);
  }

  // start somewhere, end correctly
  for (int i = offset_in_cluster; i < cluster_size_; i++) {
    if (written_bytes >= data_to_write.size()) {
      break; // already wrote everything
    }

    raw[static_cast<size_t>(i)] = data_to_write[written_bytes];
    written_bytes++;
  }

  // write to cluster
  cluster_write(cluster_idx, reinterpret_cast<char *>(raw.data()),
                cluster_size_);
}

} // namespace jkfs
