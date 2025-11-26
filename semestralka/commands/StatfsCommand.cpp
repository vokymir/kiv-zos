#include <iostream>
#include <string>

#include "commands.hpp"

namespace jkfs {

StatfsCommand::StatfsCommand() {
  m_id = "statfs";
  m_name = "Statistics of filesystem";
  m_desc = "Print parametrics of the filesystem, such as size,\n\
| number of i-nodes, number of directories, ...";
  m_how = "statfs";
}

void StatfsCommand::execute(std::vector<std::string> &a_args) {
  std::string args = "";
  for (auto arg : a_args) {
    args += arg + " ";
  }
  std::cout << "Running " + m_name + " command, with arguments: " << args
            << std::endl;
}

} // namespace jkfs
