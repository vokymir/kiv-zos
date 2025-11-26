#include <iostream>
#include <string>

#include "commands.hpp"

namespace jkfs {

CdCommand::CdCommand() {
  id_ = "cd";
  name_ = "Change directory";
  desc_ = "Go to specified directory.";
  how_ = "cd .. // go to parent directory";
}

void CdCommand::execute(std::vector<std::string> &a_args) {
  std::string args = "";
  for (auto arg : a_args) {
    args += arg + " ";
  }
  std::cout << "Running " + name_ + " command, with arguments: " << args
            << std::endl;
}

} // namespace jkfs
