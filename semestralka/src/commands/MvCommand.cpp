#include <iostream>
#include <string>

#include "commands.hpp"

namespace jkfs {

MvCommand::MvCommand() {
  id_ = "mv";
  name_ = "Move";
  desc_ = "Move one file to another location.\n\
| Can be also used for renaming.";
  how_ = "mv s1 s2 // where s1 is source and s2 target location";
}

void MvCommand::execute_inner(const std::vector<std::string> &a_args) {
  std::string args = "";
  for (auto arg : a_args) {
    args += arg + " ";
  }
  std::cout << "Running " + name_ + " command, with arguments: " << args
            << std::endl;
}

} // namespace jkfs
