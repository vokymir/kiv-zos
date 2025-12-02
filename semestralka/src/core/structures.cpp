#include "structures.hpp"
#include <cstring>
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
     << ",\n file_size=" << i.file_size << ",\n direct1=" << i.direct[0]
     << ",\n direct2=" << i.direct[1] << ",\n direct3=" << i.direct[2]
     << ",\n direct4=" << i.direct[3] << ",\n direct5=" << i.direct[4]
     << ",\n indirect1=" << i.indirect1 << ",\n indirect2=" << i.indirect2
     << "\n }";
  return os;
}

std::ostream &operator<<(std::ostream &os, const dir_item &dit) {
  std::string name(dit.item_name);

  os << "dir_item{\n"
     << " inode=" << dit.inode << ",\n name=\"" << name << "\""
     << "\n }";

  return os;
}

bool dir_item::name_matches(const std::string &other_name) const {
  // both strings are equal up-to MAX_NAME_LEN characters
  // => if other.len < MAX_NAME_LEN only other.len are compared
  bool string_equals =
      std::strncmp(item_name, other_name.c_str(), MAX_NAME_LEN) == 0;

  // item name is not longer than other_name
  bool item_name_not_longer = item_name[other_name.size()] == '\0';

  return string_equals && item_name_not_longer;
}

bool dir_item::empty() const { return item_name[0] == '\0'; }

} // namespace jkfs
