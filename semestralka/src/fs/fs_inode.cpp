#include "filesystem.hpp"

namespace jkfs {

struct inode Filesystem::inode_read(int32_t id) {
  if (id < 0) {
    throw std::logic_error(
        "Inodes have IDs from range <0,inf), but you tried " +
        std::to_string(id));
  }
  if (!inode_is_empty(id)) {
    throw std::logic_error("Inode with id=" + std::to_string(id) +
                           " is not used.");
  }

  auto sb = superblock();
  int32_t position = sb.inode_start_addr + id * sb.inode_size;

  return read<struct inode>(position);
}

int32_t Filesystem::inode_alloc() {
  auto sb = superblock();
  auto buf =
      read_bytes(static_cast<size_t>(sb.bitmapi_size), sb.bitmapi_start_addr);

  // find empty
  auto idx = get_first_bit(buf, 0);
  if (idx < 0) {
    return -1;
  }

  // mark as used & clear
  inode_write(idx, inode{});

  return idx;
}

bool Filesystem::inode_is_empty(int32_t id) {
  if (id < 0) {
    throw std::logic_error(
        "Inodes have IDs from range <0,inf), but you tried " +
        std::to_string(id));
  }

  auto sb = superblock();
  int byte_idx = id / 8;
  int bit_idx = id % 8;

  auto byte = read<uint8_t>(sb.bitmapi_start_addr + byte_idx);

  auto used = (byte >> bit_idx) & 1u;
  return !used;
}

void Filesystem::inode_write(int32_t id, const struct inode &i) {
  if (id < 0) {
    throw std::logic_error(
        "Inodes have IDs from range <0,inf), but you tried " +
        std::to_string(id));
  }

  auto sb = superblock();

  // bitmap
  int byte_idx = id / 8;
  int bit_idx = id % 8;

  uint8_t byte = read<uint8_t>(sb.bitmapi_start_addr + byte_idx);
  byte |= (1u << bit_idx); // mark as used
  write(byte, sb.bitmapi_start_addr + byte_idx);

  // inode
  write(i, sb.inode_start_addr + sb.inode_size * id);
}

void Filesystem::inode_free(int32_t id) {
  if (id < 0) {
    throw std::logic_error(
        "Inodes have IDs from range <0,inf), but you tried " +
        std::to_string(id));
  }

  auto sb = superblock();
  int byte_idx = id / 8;
  int bit_idx = id % 8;

  uint8_t byte = read<uint8_t>(sb.bitmapi_start_addr + byte_idx);
  uint8_t mask = static_cast<uint8_t>(1u << bit_idx);
  byte &= ~mask; // mark as unused
  write(byte, sb.bitmapi_start_addr + byte_idx);
}

} // namespace jkfs
