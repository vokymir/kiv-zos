#pragma once

#include <cstdint>
#include <cstring>
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
  int32_t direct1 = 0;
  int32_t direct2 = 0;
  int32_t direct3 = 0;
  int32_t direct4 = 0;
  int32_t direct5 = 0;
  int32_t indirect1 = 0;
  int32_t indirect2 = 0;
};

// write inode to stream
std::ostream &operator<<(std::ostream &os, const inode &i);

struct dir_item {
  static constexpr int MAX_NAME_LEN = 11; // 8 + 3 (+ \0 )
  int32_t inode = 0;
  char item_name[MAX_NAME_LEN + 1] = {};

  // only uses first MAX_NAME_LEN characters as item_name
  dir_item(int32_t inode_id, std::string name) : inode(inode_id) {
    memset(item_name, 0, sizeof(item_name));
    strncpy(item_name, name.c_str(), sizeof(item_name) - 1);
  }
  dir_item() {}

  // if any given name match the dir_item.item_name
  bool name_matches(const std::string &name) const;
};

// write directory item to stream
std::ostream &operator<<(std::ostream &os, const dir_item &dit);

} // namespace jkfs
