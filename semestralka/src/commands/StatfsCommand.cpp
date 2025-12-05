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
  std::cout << fs_.superblock() << std::endl;
}

} // namespace jkfs
