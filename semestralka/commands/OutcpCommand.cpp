#include <iostream>
#include <string>

#include "commands.hpp"

namespace jkfs {

OutcpCommand::OutcpCommand() {
  id_ = "outcp";
  name_ = "Copy out of the filesystem";
  desc_ = "Copy the file from inside the filesystem to outside.";
  how_ = "outcp inside_path.txt outside_path.txt";
}

void OutcpCommand::execute(std::vector<std::string> &a_args) noexcept {
  std::string args = "";
  for (auto arg : a_args) {
    args += arg + " ";
  }
  std::cout << "Running " + name_ + " command, with arguments: " << args
            << std::endl;
}

} // namespace jkfs
