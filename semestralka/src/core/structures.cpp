#include "structures.hpp"
#include <cstring>
#include <ostream>
#include <string>

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
     << ",\n file_size=" << i.file_size << ",\n direct1=" << i.direct[0]
     << ",\n direct2=" << i.direct[1] << ",\n direct3=" << i.direct[2]
     << ",\n direct4=" << i.direct[3] << ",\n direct5=" << i.direct[4]
     << ",\n indirect1=" << i.indirect1 << ",\n indirect2=" << i.indirect2
     << "\n }";
  return os;
}

std::ostream &operator<<(std::ostream &os, const dir_item &dit) {
  os << "dir_item{\n"
     << " inode=" << dit.inode << ",\n name=\"" << dit.item_name.data() << "\""
     << "\n }";

  return os;
}

bool dir_item::name_matches(const std::string &other_name) const {
  std::string_view stored(item_name.data(),
                          std::char_traits<char>::length(item_name.data()));

  return stored == other_name;
}

bool dir_item::empty() const { return item_name[0] == '\0'; }

} // namespace jkfs
