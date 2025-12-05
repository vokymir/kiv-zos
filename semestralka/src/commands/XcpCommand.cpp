#include <iostream>
#include <string>

#include "commands.hpp"

namespace jkfs {

XcpCommand::XcpCommand() {
  id_ = "xcp";
  name_ = "Join and Copy";
  desc_ = "Join contents of two files and copy them to target location.";
  how_ = "xcp s1 s2 s3 // where s1, s2 are source and s3 target location";
}

void XcpCommand::execute_inner(const std::vector<std::string> &a_args) {
  std::string args = "";
  for (auto arg : a_args) {
    args += arg + " ";
  }
  std::cout << "Running " + name_ + " command, with arguments: " << args
            << std::endl;
}

} // namespace jkfs
