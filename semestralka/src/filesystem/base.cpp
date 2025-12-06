#include "errors.hpp"
#include "filesystem.hpp"
#include "structures.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <ios>
#include <iostream>
#include <iterator>
#include <mutex>
#include <string>
#include <vector>

namespace jkfs {

// ===== singleton variables =====
Filesystem *Filesystem::instance_{nullptr};
std::mutex Filesystem::mutex_;

// ===== singleton behaviour =====
Filesystem::Filesystem(const std::string &filename) { path(filename); }

Filesystem &Filesystem::instance(const std::string &filename) {
  std::lock_guard<std::mutex> lock(mutex_);
  if (instance_ == nullptr) {
    instance_ = new Filesystem(filename);
  }
  return *instance_;
}

Filesystem &Filesystem::instance() {
  std::lock_guard<std::mutex> lock(mutex_);
  if (instance_ == nullptr) {
    throw jkfilesystem_error(
        "Cannot get singleton instance if it wasn't initialized.");
  }
  return *instance_;
}

// ===== get/set =====

bool Filesystem::vocal() const { return vocal_; }
void Filesystem::vocal(bool vocal) { vocal_ = vocal; }

std::string Filesystem::path() const { return path_; }
void Filesystem::path(const std::string &path) {
  path_ = path;
  file_.close();
  file_ = std::fstream(path_, std::ios::binary | std::ios::in | std::ios::out);
}

std::fstream &Filesystem::file() { return file_; }

struct superblock Filesystem::superblock() {
  return read<struct superblock>(0);
}

void Filesystem::superblock(const struct superblock &sb) {
  write<struct superblock>(sb, 0);
  clear_bitmaps(sb);
}

// insider knowledge - roots inode ID is always 0
int32_t Filesystem::root_id() { return 0; }
struct inode Filesystem::root_inode() { return inode_read(root_id()); }

std::vector<int32_t> Filesystem::current_directory() { return cwd_; }

void Filesystem::current_directory(int32_t inode_id) {
  // check if is really directory
  auto inode = inode_read(inode_id);
  if (!inode.is_dir) {
    throw jkfilesystem_error(
        "Cannot set cwd to inode which is not a directory.");
  }
  // find directory in existing tree
  auto it = std::ranges::find_if(cwd_, [inode_id](const int32_t curr_inode_id) {
    return inode_id == curr_inode_id;
  });
  if (it == cwd_.end()) {
    // find dir in leaf inodes dir_items
    auto find = dir_list(cwd_.back());
    auto it2 = std::ranges::find_if(find, [inode_id](const dir_item &child) {
      return child.inode == inode_id;
    });
    // didn't find it
    if (it2 == find.end()) {
      throw jkfilesystem_error(
          "Cannot nest cwd by more than one level at a time when only knows "
          "target inode id -> too many possibilities.");
    }
    cwd_.push_back(it2->inode);
  } else {
    cwd_.erase(std::next(it), cwd_.end());
  }
}

void Filesystem::current_directory(const std::string &path) {
  auto nwd = path_lookup(path); // next-working-directory
  if (nwd.empty()) {
    throw jkfilesystem_error("Cannot change current directory to " + path +
                             ". It is not a valid path.");
  }

  auto inode = inode_read(nwd.back());
  if (!inode.is_dir) {
    throw jkfilesystem_error("Cannot change current directory to " + path +
                             ". It is path to file.");
  }

  cwd_ = nwd;
}

// ===== methods =====

std::vector<int32_t> Filesystem::get_bitmap_idxs(int32_t start_addr,
                                                 size_t bytes_count) {
  auto bytes = read_bytes(bytes_count, start_addr);
  std::vector<int32_t> idxs;

  for (int i = 0; i < bytes.size(); i++) {
    auto byte = bytes[i];
    for (int j = 0; j < 8; j++) {
      if (bit_get(byte, j)) {
        idxs.push_back(i * 8 + j);
      }
    }
  }

  return idxs;
}

// ===== private methods =====

void Filesystem::write_bytes(const char *data, size_t count,
                             std::streamoff offset,
                             std::ios_base::seekdir way) {
  file_.clear();

  file_.seekp(offset, way);
  file_.write(data, static_cast<std::streamsize>(count));

  if (!file_) {
    throw jkfilesystem_error("Cannot write bytes into file.");
  }

  file_.flush();
}

std::vector<uint8_t> Filesystem::read_bytes(size_t count, std::streamoff offset,
                                            std::ios_base::seekdir way) {
  std::vector<uint8_t> buf(count);
  file_.clear();

  file_.seekg(offset, way);
  file_.read(reinterpret_cast<char *>(buf.data()),
             static_cast<std::streamsize>(count));

  if (!file_) {
    throw jkfilesystem_error("Cannot read bytes from file.");
  }

  return buf;
}

bool Filesystem::bit_get(uint8_t byte, uint32_t bit_index) {
  switch (BIT_ORDER) {
  case Bit_Order::LSB_FIRST:
    return (byte >> bit_index) & 1u;
  case Bit_Order::MSB_FIRST:
    return (byte >> (7 - bit_index)) & 1u;
  default:
    throw jkfilesystem_error("Unknown bit order.");
  }
}

void Filesystem::bit_set(uint8_t &byte, uint32_t bit_index) {
  uint8_t mask;

  switch (BIT_ORDER) {
  case Bit_Order::LSB_FIRST:
    mask = static_cast<uint8_t>(1u << bit_index);
    break;
  case Bit_Order::MSB_FIRST:
    mask = static_cast<uint8_t>(1u << (7 - bit_index));
    break;
  default:
    throw jkfilesystem_error("Unknown bit order.");
  }

  byte |= mask;
}

void Filesystem::bit_clear(uint8_t &byte, uint32_t bit_index) {
  uint8_t mask;

  switch (BIT_ORDER) {
  case Bit_Order::LSB_FIRST:
    mask = static_cast<uint8_t>(~(1u << bit_index));
    break;
  case Bit_Order::MSB_FIRST:
    mask = static_cast<uint8_t>(~(1u << (7 - bit_index)));
    break;
  default:
    throw jkfilesystem_error("Unknown bit order.");
  }

  byte &= mask;
}

int32_t Filesystem::get_first_bit(std::vector<uint8_t> &vec, bool value) {
  // walk all bytes
  for (size_t byte_idx = 0; byte_idx < vec.size(); byte_idx++) {
    uint8_t byte = vec[byte_idx];

    // walk all bits left to right
    // its only logical order, the physical is in bit_get
    for (int logical_bit = 0; logical_bit < 8; logical_bit++) {
      if (bit_get(byte, static_cast<uint8_t>(logical_bit)) == value) {
        return static_cast<int32_t>(byte_idx) * 8 + logical_bit;
      }
    }
  }
  return -1;
}

} // namespace jkfs
