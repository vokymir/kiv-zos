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

} // namespace jkfs
