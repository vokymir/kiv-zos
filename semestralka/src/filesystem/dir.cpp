#include "errors.hpp"
#include "filesystem.hpp"
#include "structures.hpp"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <span>
#include <string>
#include <vector>

namespace jkfs {

void Filesystem::dir_create(int32_t parent_id, std::string name) {
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
    dir_item_add(parent_id, id, name);
    parent_entry_added = true;

    // add self, parent references
    dir_item_add(id, id, ".");
    dir_item_add(id, parent_id, "..");

  } catch (...) {
    // rollback
    if (parent_entry_added) {
      dir_item_remove(parent_id, name);
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

void Filesystem::dir_item_add(int32_t id, int32_t item_id,
                              std::string item_name) {
  auto existing = dir_lookup(id, item_name);
  if (existing >= 0) {
    throw jkfilesystem_error("File with that name already exists.");
  }

  struct dir_item item{item_id, item_name};

  auto offset = inode_read(id).file_size;
  file_write(id, offset, reinterpret_cast<const char *>(&item), sizeof(item));
}

void Filesystem::dir_item_remove(int32_t parent_id, std::string item_name) {
  auto id = dir_lookup(parent_id, item_name);
  if (id < 0) {
    return; // TODO: my work here is done, but shouldn't I rather throw smth to
            // let others know also?
  }

  struct dir_item empty_item{0, ""};

  auto raw_file = file_read(id);
  // look on raw_file as on array of dir_items
  auto items =
      std::span<dir_item>(reinterpret_cast<dir_item *>(raw_file.data()),
                          raw_file.size() / sizeof(dir_item));

  // remove from the array
  size_t count = items.size();
  bool removed = false;

  for (size_t i = 0; i < count; i++) {
    // remember dir_item is removed
    if (!removed && items[i].name_matches(item_name)) {
      removed = true;
    }

    // shift items to fill the gap
    if (removed && i < count - 1) {
      items[i] = items[i + 1];
    }
  }

  // forget last item - now empty
  count--;
  // write back to file - this handles all inode.filesize changes, removing
  // clusters, ...
  file_write(id, 0, reinterpret_cast<const char *>(items.data()),
             static_cast<int>(count * sizeof(dir_item)));
}

} // namespace jkfs
