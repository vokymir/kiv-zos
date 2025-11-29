#pragma once

#include <cstdint>
#include <cstring>
#include <ostream>
#include <string>

namespace jkfs {

struct superblock {
  static constexpr int MAX_SIGN_LEN = 16;
  char signature[MAX_SIGN_LEN]; // author login
  int32_t disk_size;            // celkova velikost VFS

  int32_t bitmapi_start_addr; // bitmap i-node
  int32_t bitmapi_size;
  int32_t bitmapd_start_addr; // bitmap cluster
  int32_t bitmapd_size;

  int32_t inode_size; // i-node
  int32_t inode_count;
  int32_t inode_start_addr;

  int32_t cluster_size; // cluster
  int32_t cluster_count;
  int32_t data_start_addr;
};

// write superblock to stream
std::ostream &operator<<(std::ostream &os, const superblock &sb);

struct inode {
  int32_t node_id;
  char is_dir;
  int32_t file_size;
  int32_t direct1;
  int32_t direct2;
  int32_t direct3;
  int32_t direct4;
  int32_t direct5;
  int32_t indirect1;
  int32_t indirect2;
};

// write inode to stream
std::ostream &operator<<(std::ostream &os, const inode &i);

struct dir_item {
  static constexpr int MAX_NAME_LEN = 11; // 8 + 3 (+ \0 )
  int32_t inode;
  char item_name[MAX_NAME_LEN + 1];

  // only uses first MAX_NAME_LEN characters as item_name
  dir_item(int32_t inode_id, std::string name) : inode(inode_id) {
    memset(item_name, 0, sizeof(item_name));
    strncpy(item_name, name.c_str(), sizeof(item_name) - 1);
  }
};

// write directory item to stream
std::ostream &operator<<(std::ostream &os, const dir_item &dit);

} // namespace jkfs
