#include <iostream>
#include <string>

#include "commands.hpp"

namespace jkfs {

LsCommand::LsCommand() {
  id_ = "ls";
  name_ = "List directory";
  desc_ = "List all items in directory.";
  how_ = "ls . // lists all items in current directory";
}

void LsCommand::execute(std::vector<std::string> &a_args) {
  std::string args = "";
  for (auto arg : a_args) {
    args += arg + " ";
  }
  std::cout << "Running " + name_ + " command, with arguments: " << args
            << std::endl;
}

} // namespace jkfs
