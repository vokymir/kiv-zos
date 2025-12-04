#include "errors.hpp"
#include "filesystem.hpp"
#include "structures.hpp"
#include <algorithm>
#include <cstdint>
#include <iterator>
#include <ranges>
#include <span>
#include <string>
#include <vector>

namespace jkfs {

void Filesystem::dir_create(int32_t parent_id, std::string name) {
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
                              std::string item_name) {
  auto existing = dir_lookup(id, item_name);
  if (existing >= 0) {
    throw jkfilesystem_error("File with that name already exists.");
  }

  struct dir_item item{item_id, item_name};

  auto offset = inode_read(id).file_size;
  file_write(id, offset, reinterpret_cast<const char *>(&item), sizeof(item));
}

void Filesystem::dir_item_remove(int32_t id, std::string item_name) {
  auto items = dir_list(id);

  // find the item to remove
  auto it = std::ranges::find_if(items, [&item_name](const dir_item &item) {
    return item.name_matches(item_name);
  });
  if (it == items.end()) {
    return; // the work is already done
  }
  // remove the found item
  items.erase(it);

  // write back to file - this handles all inode.filesize changes, removing
  // clusters, ...
  auto data_size = items.size() * sizeof(dir_item);
  file_write(id, 0, reinterpret_cast<const char *>(items.data()), data_size);
}

int32_t Filesystem::dir_lookup(int32_t id, std::string lookup_name) {
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

  // go through raw_file
  for (std::size_t offset = 0; offset + sizeof(dir_item) <= raw_file.size();
       offset += sizeof(dir_item)) {

    dir_item item{}; // zero-initialized
                     // copy only one item
    std::copy_n(reinterpret_cast<const char *>(&raw_file[offset]),
                sizeof(dir_item), reinterpret_cast<char *>(&item));

    // only insert valid entries
    if (!item.empty()) {
      items.push_back(std::move(item));
    }
  }

  return items;
}

} // namespace jkfs
