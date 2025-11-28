#include "filesystem.hpp"
#include "structures.hpp"
#include <algorithm>
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
    throw std::runtime_error("The size is too small. Min size: " +
                             std::to_string(min_size_));
  } else if (size > max_size_) {
    throw std::runtime_error("The size is too big. Max size: " +
                             std::to_string(max_size_));
  }

  ensure_file();

  file_.close();
  std::filesystem::resize_file(path_, static_cast<uintmax_t>(size));
  path(path_);

  superblock(sb_from_size(static_cast<int32_t>(size)));
}

// ===== private methods =====

// return max count of items if each item must have record in bitmap
int32_t iterative_count_max(int32_t available_space, int32_t item_size) {
  // optimistic approach: as many items fit inside available space
  int item_count = static_cast<int>(std::floor(available_space / item_size));

  while (true) {
    int size_items = item_count * item_size;
    // 1/8 = 0.125 for 1 bit in 1 byte. ceiling because even though fewer bits
    // aren't whole byte, it must be saved in one
    int size_bitmap = static_cast<int>(std::ceil(item_count * 0.125));

    // try until it fits inside available_space
    if (size_items + size_bitmap <= available_space) {
      break;
    }

    // try with less items
    item_count--;
  }

  return item_count;
}

int32_t Filesystem::count_clusters(int32_t effective_size) const {
  int size = static_cast<int>(std::floor(effective_size * (1 - id_ratio_)));

  return std::max(1, iterative_count_max(size, cluster_size_));
}

int32_t Filesystem::count_inodes(int32_t effective_size,
                                 int32_t cluster_count) const {
  int size = effective_size - cluster_size_ * cluster_count -
             static_cast<int>(std::ceil(0.125 * cluster_count));

  return iterative_count_max(size, sizeof(struct inode));
}

struct superblock Filesystem::sb_from_size(int32_t total_size) const {
  struct superblock sb{};
  // effective size which can be used to store everything except superblock
  int32_t size = total_size - static_cast<int32_t>(sizeof(struct superblock));

  memcpy(sb.signature, "javok", 5);
  sb.disk_size = total_size;
  sb.cluster_size = cluster_size_;
  sb.inode_size = static_cast<int32_t>(sizeof(struct inode));
  sb.cluster_count = count_clusters(size);
  sb.inode_count = count_inodes(size, sb.cluster_count);

  int32_t position = sizeof(struct superblock);

  sb.bitmapi_start_addr = position;
  position +=
      static_cast<int32_t>(std::ceil(static_cast<float>(sb.inode_count) / 8.f));

  sb.bitmapd_start_addr = position;
  position += static_cast<int32_t>(
      std::ceil(static_cast<float>(sb.cluster_count) / 8.f));

  sb.inode_start_addr = position;
  position += sb.inode_count * sb.inode_size;

  sb.data_start_addr = position;
  position += sb.cluster_count * sb.cluster_size;

  std::cout << sb << std::endl; // TODO: move 1 line down in prod
  double total_s = total_size;
  std::cout << "Used space of file: \n"
            << "Total: " << std::to_string(position / total_s * 100.) << "%\n"
            << "Superblock: "
            << std::to_string(sizeof(struct superblock) / total_s * 100.)
            << "%\n"
            << "Bitmap - inodes: "
            << std::to_string(sb.inode_count / 8. / total_s * 100.) << "%\n"
            << "Bitmap - clusters: "
            << std::to_string(sb.cluster_count / 8. / total_s * 100.) << "%\n"
            << "Inodes: "
            << std::to_string(sb.inode_count * sb.inode_size / total_s * 100.)
            << "%\n"
            << "Clusters: "
            << std::to_string(sb.cluster_count * sb.cluster_size / total_s *
                              100.)
            << "%\n"
            << "Inode / Cluster ratio: " << std::to_string(id_ratio_) << "%"
            << std::endl;

  if (position > total_size) {

    throw std::runtime_error(
        "MATH ERROR: Superblock did math wrong and require more space than is "
        "available in the file... off by: " +
        std::to_string(position - total_size));
  }

  return sb;
}

} // namespace jkfs
