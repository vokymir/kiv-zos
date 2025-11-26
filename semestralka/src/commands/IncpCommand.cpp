#include <iostream>
#include <string>

#include "commands.hpp"

namespace jkfs {

IncpCommand::IncpCommand() {
  id_ = "incp";
  name_ = "Copy into filesystem";
  desc_ = "Copy the file from outside to this filesystem.";
  how_ = "incp outside_path.txt inside_path.txt";
}

void IncpCommand::execute(std::vector<std::string> &a_args) noexcept {
  std::string args = "";
  for (auto arg : a_args) {
    args += arg + " ";
  }
  std::cout << "Running " + name_ + " command, with arguments: " << args
            << std::endl;
}

} // namespace jkfs
