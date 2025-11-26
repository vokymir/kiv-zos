#include <iostream>
#include <string>

#include "commands.hpp"

namespace jkfs {

CpCommand::CpCommand() {
  id_ = "cp";
  name_ = "Copy";
  desc_ = "Copy one file to another place.";
  how_ = "cp s1 s2 // where s1 is source and s2 target location";
  exmp_ = {"cp s1 s2", "cp s2 s2"};
}

void CpCommand::execute(std::vector<std::string> &a_args) {
  std::string args = "";
  for (auto arg : a_args) {
    args += arg + " ";
  }
  std::cout << "Running " + name_ + " command, with arguments: " << args
            << std::endl;
}

} // namespace jkfs
