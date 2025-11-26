#include <iostream>
#include <string>

#include "commands.hpp"

namespace jkfs {

RmCommand::RmCommand() {
  id_ = "rm";
  name_ = "Remove";
  desc_ = "Remove file if exists.";
  how_ = "rm file.txt";
}

void RmCommand::execute(std::vector<std::string> &a_args) noexcept {
  std::string args = "";
  for (auto arg : a_args) {
    args += arg + " ";
  }
  std::cout << "Running " + name_ + " command, with arguments: " << args
            << std::endl;
}

} // namespace jkfs
