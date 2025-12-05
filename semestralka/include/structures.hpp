#pragma once

#include <algorithm>
#include <cstdint>
#include <ostream>
#include <string>

namespace jkfs {

struct superblock {
  static constexpr int MAX_SIGN_LEN = 16;
  char signature[MAX_SIGN_LEN] = {}; // author login
  int32_t disk_size = 0;             // celkova velikost VFS

  int32_t bitmapi_start_addr = 0; // bitmap i-node
  int32_t bitmapi_size = 0;
  int32_t bitmapd_start_addr = 0; // bitmap cluster
  int32_t bitmapd_size = 0;

  int32_t inode_size = 0; // i-node
  int32_t inode_count = 0;
  int32_t inode_start_addr = 0;

  int32_t cluster_size = 0; // cluster
  int32_t cluster_count = 0;
  int32_t data_start_addr = 0;
};

// write superblock to stream
std::ostream &operator<<(std::ostream &os, const superblock &sb);

struct inode {
  int32_t node_id = 0;
  char is_dir = 0;
  int32_t file_size = 0;
  int32_t direct[5] = {0};
  int32_t indirect1 = 0;
  int32_t indirect2 = 0;

  // set all member values to be the same as in other
  void copy_from(const inode &other);
};

// write inode to stream
std::ostream &operator<<(std::ostream &os, const inode &i);

struct dir_item {
  static constexpr uint8_t MAX_NAME_LEN = 11; // 8 + 3 (+ \0 )
  int32_t inode = 0;
  std::array<char, MAX_NAME_LEN + 1> item_name{};

  // only uses first MAX_NAME_LEN characters as item_name
  dir_item(int32_t inode_id, std::string name) : inode(inode_id) {
    const auto n = std::min(static_cast<uint8_t>(name.size()), MAX_NAME_LEN);
    std::copy_n(name.data(), n, item_name.data());
    item_name[n] = '\0'; // zero-terminate
  }
  dir_item() {}

  // if any given name match the dir_item.item_name
  bool name_matches(const std::string &name) const;
  // return if dir_item is empty
  bool empty() const;
};

// write directory item to stream
std::ostream &operator<<(std::ostream &os, const dir_item &dit);

} // namespace jkfs
