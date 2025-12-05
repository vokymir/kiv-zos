#include "structures.hpp"
#include <iterator>
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
     << ",\n is_dir=" << (i.is_dir ? "true" : "false")
     << ",\n file_size=" << i.file_size << "b,\n direct1=" << i.direct[0]
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

void inode::copy_from(const inode &o) {
  node_id = o.node_id;
  is_dir = o.is_dir;
  file_size = o.file_size;
  for (auto i = 0; i < std::size(direct); i++) {
    direct[i] = o.direct[i];
  }
  indirect1 = o.indirect1;
  indirect2 = o.indirect2;
}

bool dir_item::name_matches(const std::string &other_name) const {
  std::string_view stored(item_name.data(),
                          std::char_traits<char>::length(item_name.data()));

  return stored == other_name;
}

bool dir_item::empty() const { return item_name[0] == '\0'; }

bool dir_item::operator<(const dir_item &other) const {
  return std::string(item_name.data()) < std::string(other.item_name.data());
}

} // namespace jkfs
