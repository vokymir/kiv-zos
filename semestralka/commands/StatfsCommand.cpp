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

void StatfsCommand::execute(std::vector<std::string> &a_args) {
  std::string args = "";
  for (auto arg : a_args) {
    args += arg + " ";
  }
  std::cout << "Running " + name_ + " command, with arguments: " << args
            << std::endl;
}

} // namespace jkfs
