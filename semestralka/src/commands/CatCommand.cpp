#include <iostream>
#include <string>

#include "commands.hpp"

namespace jkfs {

CatCommand::CatCommand() {
  id_ = "cat";
  name_ = "Concatenate";
  desc_ = "Print the contents of one file.";
  how_ = "cat file.txt";
}

void CatCommand::execute_inner(std::vector<std::string> &a_args) {
  std::string args = "";
  for (auto arg : a_args) {
    args += arg + " ";
  }
  std::cout << "Running " + name_ + " command, with arguments: " << args
            << std::endl;
}

} // namespace jkfs
