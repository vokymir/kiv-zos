#include "errors.hpp"
#include "filesystem.hpp"
#include "structures.hpp"
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <vector>

namespace jkfs {

// === METHODS ===

void Filesystem::filesystem_ensure() {
  if (!std::filesystem::exists(path_)) {
    file_.close();
    std::ofstream o(path_);
    o.close();
    path(path_);
  }
}

void Filesystem::filesystem_resize(size_t size) {
  if (size < min_size_) {
    throw jkfilesystem_error("The size is too small. Min size: " +
                             std::to_string(min_size_));
  } else if (size > max_size_) {
    throw jkfilesystem_error("The size is too big. Max size: " +
                             std::to_string(max_size_));
  }

  // if this fails, rather fail before resizing
  auto sb = sb_from_size(static_cast<int32_t>(size));

  filesystem_ensure();

  file_.close();
  std::filesystem::resize_file(path_, static_cast<uintmax_t>(size));
  path(path_);

  superblock(sb);

  // set the first cluster as used - because idx 0 is reserved for "INVALID
  // CLUSTER"
  cluster_alloc();

  // root is its own parent
  dir_create(0, "/");
}

// === PRIVATE METHODS ===

int32_t Filesystem::iterative_count_max(int32_t available_space,
                                        int32_t item_size) const {
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

  return std::max(2, iterative_count_max(size, cluster_size_));
}

int32_t Filesystem::count_inodes(int32_t effective_size,
                                 int32_t cluster_count) const {
  int size = effective_size - cluster_size_ * cluster_count -
             static_cast<int>(std::ceil(0.125 * cluster_count));

  return iterative_count_max(size, sizeof(struct inode));
}

void Filesystem::print_usage_info(struct superblock &sb) const {
  double ts = sb.disk_size;
  double position = sb.data_start_addr + cluster_size_ * sb.cluster_count;
  std::cout << "Used space of file: \n"
            << "Total: " << std::to_string(position / ts * 100.) << "%\n"
            << "Superblock: "
            << std::to_string(sizeof(struct superblock) / ts * 100.) << "%\n"
            << "Bitmap - inodes: "
            << std::to_string(sb.bitmapi_size / ts * 100.) << "%\n"
            << "Bitmap - clusters: "
            << std::to_string(sb.bitmapd_size / ts * 100.) << "%\n"
            << "Inodes: "
            << std::to_string(sb.inode_count * sb.inode_size / ts * 100.)
            << "%\n"
            << "Clusters: "
            << std::to_string(sb.cluster_count * sb.cluster_size / ts * 100.)
            << "%\n"
            << "Inode / Cluster ratio: " << std::to_string(id_ratio_) << "%"
            << std::endl;
}

struct superblock Filesystem::sb_from_size(int32_t total_size) const {
  struct superblock sb{};
  // effective size which can be used to store everything except superblock
  int32_t size = total_size - static_cast<int32_t>(sizeof(struct superblock));

  std::copy_n("javok", 5, sb.signature);
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

  sb.bitmapi_size = sb.bitmapd_start_addr - sb.bitmapi_start_addr;
  sb.bitmapd_size = sb.inode_start_addr - sb.bitmapd_start_addr;

  if (vocal_) {
    std::cout << sb << std::endl;
    print_usage_info(sb);
  }

  if (position > total_size) {
    throw jkfilesystem_error(
        "MATH ERROR: Superblock did math wrong and require more space than is "
        "available in the file... off by: " +
        std::to_string(position - total_size));
  }

  return sb;
}

void Filesystem::clear_bitmaps(const struct superblock &sb) {
  std::vector<uint8_t> zeros;

  zeros.assign(static_cast<size_t>(sb.bitmapi_size), 0);
  write_bytes(reinterpret_cast<const char *>(zeros.data()), sb.bitmapi_size,
              sb.bitmapi_start_addr);

  zeros.assign(static_cast<size_t>(sb.bitmapd_size), 0);
  write_bytes(reinterpret_cast<const char *>(zeros.data()), sb.bitmapd_size,
              sb.bitmapd_start_addr);
}

} // namespace jkfs
