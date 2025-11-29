#include "errors.hpp"
#include "filesystem.hpp"
#include "structures.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iterator>
#include <ranges>
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

void Filesystem::dir_item_remove(int32_t id, std::string item_name) {
  auto raw_file = file_read(id);
  // look on raw_file as on array of dir_items
  auto items =
      std::span<dir_item>(reinterpret_cast<dir_item *>(raw_file.data()),
                          raw_file.size() / sizeof(dir_item));

  // find the item to remove
  auto it = std::ranges::find_if(items, [&](const dir_item &item) {
    return item.name_matches(item_name);
  });
  if (it == items.end()) {
    return; // TODO: or throw smth???
  }

  auto index = std::distance(items.begin(), it);

  // shift all items after found one to the left & set last item to nothing
  std::ranges::copy_backward(
      items.subspan(static_cast<size_t>(index + 1)),
      items
          .subspan(static_cast<size_t>(index + 1),
                   items.size() - (static_cast<size_t>(index + 1)) + 1)
          .end());
  items.back() = dir_item{};

  // forget last item - now empty (therefore -1)
  // write back to file - this handles all inode.filesize changes, removing
  // clusters, ...
  file_write(id, 0, reinterpret_cast<const char *>(items.data()),
             static_cast<int>((items.size() - 1) * sizeof(dir_item)));
}

int32_t Filesystem::dir_lookup(int32_t id, std::string lookup_name) {
  auto raw_file = file_read(id);
  // view the file as array of dir_items
  auto items =
      std::span<dir_item>(reinterpret_cast<dir_item *>(raw_file.data()),
                          raw_file.size() / sizeof(dir_item));
  // find the matching name
  auto it = std::ranges::find_if(items, [&](const dir_item &item) {
    return item.name_matches(lookup_name);
  });

  if (it == items.end()) {
    return -1;
  }
  return it->inode;
}

std::vector<dir_item> Filesystem::dir_list(int32_t id) {
  auto raw_file = file_read(id);
  // view the file as array of dir_items
  auto items =
      std::span<dir_item>(reinterpret_cast<dir_item *>(raw_file.data()),
                          raw_file.size() / sizeof(dir_item));

  // filter-out empty entries
  auto view = items | std::ranges::views::filter(
                          [](const dir_item &item) { return !item.empty(); });

  // collect to vector
  std::vector<dir_item> result;
  std::ranges::copy(view, std::back_inserter(result));
  return result;
}

} // namespace jkfs
