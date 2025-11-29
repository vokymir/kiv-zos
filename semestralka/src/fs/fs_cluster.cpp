#include "filesystem.hpp"
#include <cstring>

namespace jkfs {

std::vector<uint8_t> Filesystem::cluster_read(int32_t idx) {
  if (idx < 0) {
    throw std::logic_error(
        "Clusters are indexed from 0 upwards, but you tried " +
        std::to_string(idx));
  }

  auto sb = superblock();
  auto offset = sb.data_start_addr + idx * sb.cluster_size;

  return read_bytes(static_cast<size_t>(sb.cluster_size), offset);
}

int32_t Filesystem::cluster_alloc() {
  auto sb = superblock();
  auto buf =
      read_bytes(static_cast<size_t>(sb.bitmapd_size), sb.bitmapd_start_addr);

  // find
  auto idx = get_first_bit(buf, 0);
  if (idx < 0) {
    return -1;
  }

  // clear & mark as used
  cluster_write(idx, nullptr, 0);

  return idx;
}

bool Filesystem::cluster_is_empty(int32_t idx) {
  if (idx < 0) {
    throw std::logic_error(
        "Clusters are indexed from 0 upwards, but you tried " +
        std::to_string(idx));
  }

  auto sb = superblock();
  int byte_idx = idx / 8;
  int bit_idx = idx % 8;

  auto byte = read<uint8_t>(sb.bitmapd_start_addr + byte_idx);

  auto used = (byte >> bit_idx) & 1u;
  return !used;
}
void Filesystem::cluster_write(int32_t idx, const char *data, int32_t size) {
  if (idx < 0) {
    throw std::logic_error(
        "Clusters are indexed from 0 upwards, but you tried " +
        std::to_string(idx));
  }

  auto sb = superblock();

  // bitmap
  int byte_idx = idx / 8;
  int bit_idx = idx % 8;

  uint8_t byte = read<uint8_t>(sb.bitmapd_start_addr + byte_idx);
  byte |= (1u << bit_idx); // mark as used
  write(byte, sb.bitmapd_start_addr + byte_idx);

  // cluster
  // zero-initialized
  std::vector<char> buf(static_cast<size_t>(sb.cluster_size), 0);
  if (!(data == nullptr || size <= 0)) {
    // copy actual data
    memcpy(buf.data(), data, static_cast<size_t>(size));
  }
  // write into FS
  write_bytes(buf.data(), static_cast<size_t>(sb.cluster_size),
              sb.data_start_addr + sb.cluster_size * idx);
}

void Filesystem::cluster_free(int32_t idx) {
  if (idx < 0) {
    throw std::logic_error(
        "Clusters are indexed from 0 upwards, but you tried " +
        std::to_string(idx));
  }

  auto sb = superblock();
  int byte_idx = idx / 8;
  int bit_idx = idx % 8;

  auto byte = read<uint8_t>(sb.bitmapd_start_addr + byte_idx);
  uint8_t mask = static_cast<uint8_t>(1u << bit_idx);
  byte &= ~mask; // mark as unused
  write(byte, sb.bitmapd_start_addr + byte_idx);
}

} // namespace jkfs
