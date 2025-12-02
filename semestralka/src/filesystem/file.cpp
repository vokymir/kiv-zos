#include "errors.hpp"
#include "filesystem.hpp"
#include "structures.hpp"
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

} // namespace jkfs
