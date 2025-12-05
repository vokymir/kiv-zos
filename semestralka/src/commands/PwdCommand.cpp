#include <iostream>
#include <string>

#include "commands.hpp"

namespace jkfs {

PwdCommand::PwdCommand() {
  id_ = "pwd";
  name_ = "Print working directory";
  desc_ = "Display the full name of current directory, from the 'root'.";
  how_ = "pwd";
}

void PwdCommand::execute_inner(const std::vector<std::string> &a_args) {
  std::string args = "";
  for (auto arg : a_args) {
    args += arg + " ";
  }
  std::cout << "Running " + name_ + " command, with arguments: " << args
            << std::endl;
}

} // namespace jkfs
