#include "errors.hpp"
#include "filesystem.hpp"
#include "structures.hpp"
#include <cstdint>
#include <string>

namespace jkfs {

void Filesystem::dir_create(int32_t parent_inode, std::string name) {
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
    struct inode dir{};
    dir.node_id = id;
    dir.is_dir = true;
    dir.file_size = 0;
    dir.direct1 = cluster;

    // write inode
    inode_write(id, dir);

    // add this dir to parent
    dir_item_add(parent_inode, id, name);
    parent_entry_added = true;

    // add self, parent references
    dir_item_add(id, id, ".");
    dir_item_add(id, parent_inode, "..");

  } catch (...) {
    // rollback
    if (parent_entry_added) {
      dir_item_remove(parent_inode, name);
    }
    if (cluster >= 0) {
      cluster_free(cluster);
    }
    if (id >= 0) {
      inode_free(id);
    }

    // let others know about the exception
    throw;
  }
}

} // namespace jkfs
