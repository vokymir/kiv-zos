#include "filesystem.hpp"
#include "structures.hpp"
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <ios>
#include <iostream>
#include <mutex>
#include <stdexcept>
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
    throw std::runtime_error(
        "Cannot get singleton instance if it wasn't initialized.");
  }
  return *instance_;
}

// ===== get/set =====
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
}

// ===== methods =====

// == inodes ==

struct inode Filesystem::inode_get(int32_t id) {
  int idx = id - 1;
  if (idx < 0) {
    throw std::logic_error(
        "Inodes have IDs from range <1,inf), but you tried " +
        std::to_string(id));
  }
  if (!inode_is_empty(id)) {
    throw std::logic_error("Inode with id=" + std::to_string(id) +
                           " is not used.");
  }

  auto sb = read<struct superblock>(0);
  int32_t position = sb.inode_start_addr + idx * sb.inode_size;

  return read<struct inode>(position);
}

int32_t Filesystem::inode_get_empty() {
  auto sb = read<struct superblock>(0);
  auto buf = read_bytes(
      static_cast<size_t>(sb.bitmapd_start_addr - sb.bitmapi_start_addr),
      sb.bitmapi_start_addr);

  auto idx = get_first_bit(buf, 0);
  return idx + 1; // because IDs are 1-indexed

  // and if none bit was found, than return -1 + 1 = 0 which is declared error
}

bool Filesystem::inode_is_empty(int32_t id) {
  auto sb = read<struct superblock>(0);

  int idx = id - 1;
  int byte_idx = idx / 8;
  int bit_idx = idx % 8;

  auto byte = read<uint8_t>(sb.bitmapi_start_addr + byte_idx);

  auto used = (byte >> bit_idx) & 1u;
  return !used;
}

void Filesystem::inode_write(int32_t id, const struct inode &i) {
  auto sb = read<struct superblock>(0);
  int idx = id - 1;

  // bitmap
  int byte_idx = idx / 8;
  int bit_idx = idx % 8;

  uint8_t byte = read<uint8_t>(sb.bitmapi_start_addr + byte_idx);
  byte |= (1u << bit_idx); // mark as used
  write(byte, sb.bitmapi_start_addr + byte_idx);

  // inode
  write(i, sb.inode_start_addr + sb.inode_size * idx);
}

void Filesystem::inode_free(int32_t id) {
  auto sb = read<struct superblock>(0);
  int idx = id - 1;
  int byte_idx = idx / 8;
  int bit_idx = idx % 8;

  uint8_t byte = read<uint8_t>(sb.bitmapi_start_addr + byte_idx);
  byte &= ~(1u << bit_idx); // mark as unused
  write(byte, sb.bitmapi_start_addr + byte_idx);
}

// ===== private methods =====

std::vector<uint8_t> Filesystem::read_bytes(size_t count, std::streamoff offset,
                                            std::ios_base::seekdir way) {
  std::vector<uint8_t> buf(count);
  file_.clear();

  file_.seekg(offset, way);
  file_.read(reinterpret_cast<char *>(buf.data()),
             static_cast<std::streamsize>(count));

  if (!file_) {
    throw std::runtime_error("Cannot read from file.");
  }

  return buf;
}

int32_t get_first_bit(std::vector<uint8_t> &vec, bool value) {
  // walk all bytes
  for (size_t byte_idx = 0; byte_idx < vec.size(); byte_idx++) {
    uint8_t byte = vec[byte_idx];

    // walk all bits left to right
    for (int bit = 7; bit >= 0; bit--) {
      bool bit_val = (byte >> bit) & 1u;

      if (bit_val == value) {
        return static_cast<int32_t>(byte_idx) * 8 + (7 - bit);
      }
    }
  }
  return -1;
}

} // namespace jkfs
