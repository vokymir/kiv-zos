#pragma once

#include <cstdint>
#include <ostream>

namespace jkfs {

constexpr int32_t ITEM_FREE = 0;

struct superblock {
  char signature[16]; // author login
  int32_t disk_size;  // celkova velikost VFS
  int32_t cluster_size;
  int32_t cluster_count;
  int32_t inode_size;
  int32_t inode_count;
  int32_t bitmapi_start_addr;
  int32_t bitmapd_start_addr;
  int32_t inode_start_addr;
  int32_t data_start_addr;
};

// write superblock to stream
std::ostream &operator<<(std::ostream &os, const superblock &sb);

struct inode {
  int32_t node_id; // if == ITEM_FREE, inode is not used
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

struct dir_item {
  int32_t inode;
  char item_name[12]; // 8 + 3 + \0
};

} // namespace jkfs
