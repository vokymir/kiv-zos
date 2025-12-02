#include "errors.hpp"
#include "filesystem.hpp"
#include "structures.hpp"
#include <cstddef>
#include <cstdint>

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
    file.direct1 = cluster;

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

// TODO:
void Filesystem::file_resize(int32_t inode_id, int32_t new_size) {}

// TODO:
void Filesystem::file_write(int32_t inode_id, int32_t offset, const char *data,
                            int32_t data_size) {}

// TODO:
std::vector<uint8_t> Filesystem::file_read(int32_t inode_id) { return {}; }

// TODO:
void Filesystem::file_delete(int32_t parent_inode_id, std::string file_name) {}

// PRIVATE

std::vector<int32_t> Filesystem::file_list_clusters(int32_t inode_id) {
  std::vector<int32_t> clusters;
  auto inode = inode_read(inode_id);

  // direct 1
  int32_t cluster_idx = inode.direct1;
  if (cluster_idx <= 0) {
    return clusters;
  }
  clusters.push_back(cluster_idx);

  // direct 2
  cluster_idx = inode.direct1;
  if (cluster_idx <= 0) {
    return clusters;
  }
  clusters.push_back(cluster_idx);

  // direct 3
  cluster_idx = inode.direct1;
  if (cluster_idx <= 0) {
    return clusters;
  }
  clusters.push_back(cluster_idx);

  // direct 4
  cluster_idx = inode.direct1;
  if (cluster_idx <= 0) {
    return clusters;
  }
  clusters.push_back(cluster_idx);

  // direct 5
  cluster_idx = inode.direct1;
  if (cluster_idx <= 0) {
    return clusters;
  }
  clusters.push_back(cluster_idx);

  // indirect 1
  if (inode.indirect1 <= 0) {
    return clusters;
  }
  auto bytes_in1 = cluster_read(inode.indirect1);
  std::span<const int32_t> cluster_idxs_in1{
      reinterpret_cast<const int32_t *>(bytes_in1.data()),
      bytes_in1.size() / sizeof(int32_t)};

  for (const auto &idx : cluster_idxs_in1) {
    if (idx <= 0) {
      return clusters;
    }
    clusters.push_back(idx);
  }

  // indirect 2
  if (inode.indirect2 <= 0) {
    return clusters;
  }
  auto bytes_in2 = cluster_read(inode.indirect2);
  std::span<const int32_t> in2_pointer_2{
      reinterpret_cast<const int32_t *>(bytes_in2.data()),
      bytes_in2.size() / sizeof(int32_t)};
  for (const auto &pointer_idx : in2_pointer_2) {
    if (pointer_idx <= 0) {
      return clusters;
    }
    auto bytes_in2__ = cluster_read(pointer_idx);
    std::span<const int32_t> in2_pointer_1{
        reinterpret_cast<const int32_t *>(bytes_in2__.data()),
        bytes_in2__.size() / sizeof(int32_t)};

    for (const auto &idx : in2_pointer_1) {
      if (idx <= 0) {
        return clusters;
      }
      clusters.push_back(idx);
    }
  }

  return clusters;
}

} // namespace jkfs
