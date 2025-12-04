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

void Filesystem::file_ensure_size(int32_t inode_id, int32_t new_size) {}

void Filesystem::file_write(int32_t inode_id, int32_t offset, const char *data,
                            size_t data_size) {
  // if dont have enough space, resize
  file_ensure_size(inode_id, static_cast<int32_t>(data_size) + offset);

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

struct Needed_Clusters Filesystem::file_count_clusters(int32_t size) {
  Needed_Clusters result{};

  // ceil of size/cluster_size
  size_t data = (size + cluster_size_ - 1) / cluster_size_;
  result.data = data;

  // direct
  size_t direct = std::min(std::size(inode{}.direct), data);
  result.direct = direct;
  data -= direct;

  if (data < 0) {
    throw jkfilesystem_error("I cannot count to ten.");
  }
  if (data == 0) {
    return result; // fits inside direct clusters
  }

  // indirect 1
  size_t indirect1 = 1; // if we are still here, it is needed
  // floor of how many dir_items fit inside one cluster
  const size_t data_in_cluster = sizeof(dir_item) / cluster_size_;
  result.indirect1 = indirect1;
  data -= data_in_cluster;

  // in indirect1 can be some space left
  if (data <= 0) {
    return result;
  }

  // indirect 2
  size_t indirect2 = 1; // the same logic - we are here, so its needed
  // floor of how many indirect1 pointers fit inside one indirect2
  const size_t clusters_in_cluster = sizeof(int32_t) / cluster_size_;
  // ceil of how many indirect1s are needed inside indirect2
  size_t in1_inside_in2 = (data + data_in_cluster - 1) / data_in_cluster;

  // the number of indirect1s is too big to fit inside indirect2
  if (in1_inside_in2 > clusters_in_cluster) {
    result.possible = false;
    return result;
  }

  result.indirect1 += in1_inside_in2;
  result.indirect2 = indirect2;
  data -= data_in_cluster * in1_inside_in2;

  return result;
}

} // namespace jkfs
