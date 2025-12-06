#include <iostream>
#include <string>

#include "commands.hpp"

namespace jkfs {

StatfsCommand::StatfsCommand() {
  id_ = "statfs";
  name_ = "Statistics of filesystem";
  desc_ = "Print parametrics of the filesystem, such as size,\n\
| number of i-nodes, number of directories, ...";
  how_ = "statfs";
}

void StatfsCommand::execute_inner(
    [[maybe_unused]] const std::vector<std::string> &_) {
  auto sb = fs_.superblock();
  std::cout << sb << std::endl;
  fs_.print_usage_info(sb);

  auto bitmapi = fs_.get_bitmap_idxs(sb.bitmapi_start_addr, sb.bitmapi_size);
  auto bitmapd = fs_.get_bitmap_idxs(sb.bitmapd_start_addr, sb.bitmapd_size);

  std::cout << "Used inodes(" << bitmapi.size() << "/" << sb.inode_count
            << "): " << compress_ranges(bitmapi) << std::endl;
  std::cout << "Used clusters(" << bitmapd.size() << "/" << sb.cluster_count
            << "): " << compress_ranges(bitmapd) << std::endl;

  int dir_count = 0;
  for (const auto &inode_id : bitmapi) {
    dir_count += fs_.dir_is(inode_id);
  }
  std::cout << "Total number of directories: " << dir_count << std::endl;
}

std::string StatfsCommand::compress_ranges(const std::vector<int32_t> &v) {
  if (v.empty())
    return "";

  std::ostringstream out;
  int32_t start = v[0];
  int32_t prev = v[0];

  for (size_t i = 1; i < v.size(); ++i) {
    if (v[i] == prev + 1) {
      // still in the same range
      prev = v[i];
    } else {
      // range ended, output it
      if (start == prev)
        out << start << ", ";
      else
        out << start << "-" << prev << ", ";

      start = prev = v[i];
    }
  }

  // output last range
  if (start == prev)
    out << start;
  else
    out << start << "-" << prev;

  return out.str();
}

} // namespace jkfs
