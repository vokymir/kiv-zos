#include "errors.hpp"
#include "filesystem.hpp"
#include "structures.hpp"
#include <algorithm>
#include <cstdint>
#include <string>
#include <vector>

namespace jkfs {

void Filesystem::dir_create(int32_t parent_id, const std::string &name) {
  int32_t id = -1;

  try {
    id = file_create(parent_id, name);

    // set as directory
    auto inode = inode_read(id);
    inode.is_dir = true;
    inode_write(id, inode);

    // add self, parent references
    dir_item_add(id, id, ".");
    dir_item_add(id, parent_id, "..");

  } catch (...) {
    // rollback
    if (id >= 0) {
      file_delete(parent_id, name);
    }

    // let others know about the exception
    throw;
  }
}

void Filesystem::dir_item_add(int32_t id, int32_t item_id,
                              const std::string &item_name) {
  auto existing = dir_lookup(id, item_name);
  if (existing >= 0) {
    throw jkfilesystem_error("File with that name already exists.");
  }

  struct dir_item item{item_id, item_name};

  auto offset = inode_read(id).file_size;
  file_write(id, offset, reinterpret_cast<const char *>(&item), sizeof(item));
}

void Filesystem::dir_item_remove(int32_t id, const std::string &item_name) {
  auto items = dir_list(id);

  // find the item to remove
  auto it = std::ranges::find_if(items, [&item_name](const dir_item &item) {
    return item.name_matches(item_name);
  });
  if (it == items.end()) {
    return; // the work is already done
  }
  // ensure won't stay in removed dir
  auto cwd = current_directory();
  // start on 1, because root cannot be removed
  for (int i = 1; i < cwd.size(); i++) {
    if (cwd[i] == it->inode) {       // if this is the removed dir
      current_directory(cwd[i - 1]); // go one level up
    }
  }

  // remove the found item; if its not root :))
  if (it->inode == root_id()) {
    throw jkfilesystem_error("Cannot remove root :]");
  }
  items.erase(it);

  // write back to file - this handles all inode.filesize changes, removing
  // clusters, ...
  auto data_size = items.size() * sizeof(dir_item);
  file_write(id, 0, reinterpret_cast<const char *>(items.data()), data_size);
}

int32_t Filesystem::dir_lookup(int32_t id, const std::string &lookup_name) {
  auto items = dir_list(id);

  // find the matching name
  auto it = std::ranges::find_if(items, [&lookup_name](const dir_item &item) {
    return item.name_matches(lookup_name);
  });

  if (it == items.end()) {
    return -1;
  }
  return it->inode;
}

std::vector<dir_item> Filesystem::dir_list(int32_t id) {
  auto raw_file = file_read(id);

  // prepare space for items
  std::vector<dir_item> items;
  items.reserve(raw_file.size() / sizeof(dir_item));

  // copy all valid items from raw_file to items
  const dir_item *data = reinterpret_cast<const dir_item *>(raw_file.data());
  for (size_t i = 0; i < raw_file.size() / sizeof(dir_item); ++i) {
    if (!data[i].empty()) {
      items.push_back(data[i]);
    }
  }

  return items;
}

bool Filesystem::dir_is(int32_t inode_id) {
  return inode_read(inode_id).is_dir;
}

bool Filesystem::dir_empty(int32_t dir_inode_id) {
  auto inode = inode_read(dir_inode_id);
  // every directory have "." and ".."
  // root directory also have "/"
  return (inode.file_size <=
          sizeof(struct dir_item) * (dir_inode_id == root_id() ? 3 : 2));
}

} // namespace jkfs
