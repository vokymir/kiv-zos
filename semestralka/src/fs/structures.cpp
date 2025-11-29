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

std::ostream &operator<<(std::ostream &os, const inode &i) {
  os << "inode{\n"
     << " node_id=" << i.node_id
     << ",\n is_dir=" << static_cast<int>(i.is_dir) // print as number, not char
     << ",\n file_size=" << i.file_size << ",\n direct1=" << i.direct1
     << ",\n direct2=" << i.direct2 << ",\n direct3=" << i.direct3
     << ",\n direct4=" << i.direct4 << ",\n direct5=" << i.direct5
     << ",\n indirect1=" << i.indirect1 << ",\n indirect2=" << i.indirect2
     << "\n }";
  return os;
}

std::ostream &operator<<(std::ostream &os, const dir_item &dit) {
  // item_name may not be null-terminated, so ensure safe printing
  std::string name(dit.item_name,
                   strnlen(dit.item_name, sizeof(dit.item_name)));

  os << "dir_item{\n"
     << " inode=" << dit.inode << ",\n name=\"" << name << "\""
     << "\n }";

  return os;
}

} // namespace jkfs
