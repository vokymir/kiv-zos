#include "structures.hpp"
#include <ostream>

namespace jkfs {

std::ostream &operator<<(std::ostream &os, const superblock &sb) {
  std::string_view sig(sb.signature, sizeof sb.signature);

  os << "superblock{\n"
     << "  signature: \"" << sig << "\",\n"
     << "  disk_size: " << sb.disk_size << ",\n"
     << "  cluster_size: " << sb.cluster_size << ",\n"
     << "  cluster_count: " << sb.cluster_count << ",\n"
     << "  inode_size: " << sb.inode_size << ",\n"
     << "  inode_count: " << sb.inode_count << ",\n"
     << "  bitmapi_start_addr: " << sb.bitmapi_start_addr << ",\n"
     << "  bitmapd_start_addr: " << sb.bitmapd_start_addr << ",\n"
     << "  inode_start_addr: " << sb.inode_start_addr << ",\n"
     << "  data_start_addr: " << sb.data_start_addr << "\n"
     << "}";
  return os;
}

} // namespace jkfs
