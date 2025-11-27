#include "filesystem.hpp"
#include "structures.hpp"
#include <cmath>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <ios>
#include <iostream>
#include <mutex>
#include <stdexcept>
#include <string>

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

// get/set
std::string Filesystem::path() const { return path_; }
void Filesystem::path(const std::string &path) {
  path_ = path;
  file_.close();
  file_ = std::fstream(path_, std::ios::binary | std::ios::in | std::ios::out);
}

std::fstream &Filesystem::file() { return file_; }

struct superblock Filesystem::superblock() {
  return read<struct superblock>(0, std::ios::beg);
}

void Filesystem::superblock(const struct superblock &sb) {
  write<struct superblock>(sb, 0, std::ios::beg);
}

// ===== methods =====

void Filesystem::ensure_file() {
  if (!std::filesystem::exists(path_)) {
    file_.close();
    std::ofstream o(path_);
    o.close();
    path(path_);
  }
}

void Filesystem::resize_file(size_t size) {
  if (size < min_size_) {
    throw std::runtime_error("The size is too small.");
  } else if (size > max_size_) {
    throw std::runtime_error("The size is too big.");
  }

  ensure_file();

  file_.close();
  std::filesystem::resize_file(path_, static_cast<uintmax_t>(size));
  path(path_);

  superblock(sb_from_size(static_cast<int32_t>(size)));
}

// ===== private methods =====

int32_t Filesystem::count_inodes(int32_t effective_size) const {
  return static_cast<int32_t>(
      std::floor((static_cast<float>(effective_size) * id_ratio_) /
                 (sizeof(struct inode) + 1.f / 8.f)));
}

int32_t Filesystem::count_clusters(int32_t effective_size) const {
  return static_cast<int32_t>(
      std::floor((static_cast<float>(effective_size) * (1.f - id_ratio_)) /
                 (static_cast<float>(cluser_size_) + 1.f / 8.f)));
}

struct superblock Filesystem::sb_from_size(int32_t total_size) const {
  struct superblock sb{};
  // effective size which can be used to store everything except superblock
  int32_t size = total_size - static_cast<int32_t>(sizeof(struct superblock));

  memcpy(sb.signature, "javok", 5);
  sb.disk_size = total_size;
  sb.cluster_size = cluser_size_;
  sb.cluster_count = count_clusters(size);

  int32_t position = sizeof(struct superblock);

  sb.bitmapi_start_addr = position;
  position += static_cast<int32_t>(
      std::ceil(static_cast<float>(count_inodes(size)) / 8.f));

  sb.bitmapd_start_addr = position;
  position += static_cast<int32_t>(
      std::ceil(static_cast<float>(count_clusters(size)) / 8.f));

  sb.inode_start_addr = position;
  position += count_inodes(size) * static_cast<int32_t>(sizeof(struct inode));

  sb.data_start_addr = position;
  position += count_clusters(size) * cluser_size_;

  std::cout << sb << std::endl;
  if (position > total_size) {

    throw std::runtime_error(
        "MATH ERROR: Superblock did math wrong and require more space than is "
        "available in the file... off by: " +
        std::to_string(position - total_size));
  }

  return sb;
}

} // namespace jkfs
