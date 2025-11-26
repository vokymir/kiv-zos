#include <iostream>
#include <string>

#include "commands.hpp"

namespace jkfs {

FormatCommand::FormatCommand() {
  id_ = "format";
  name_ = "Format filesystem";
  desc_ = "Format the file given as a parameter at the start of the program.\n\
| Format to the filesystem of given size. \n\
| If file already existed (no matter if was filesystem or not) it will be overwritten,\n\
| if didn't exist it will be created.";
  how_ = "format 600MB // format the file to be filesystem of 600 Mebibyte";
}

void FormatCommand::execute(std::vector<std::string> &a_args) {
  std::string args = "";
  for (auto arg : a_args) {
    args += arg + " ";
  }
  std::cout << "Running " + name_ + " command, with arguments: " << args
            << std::endl;
}

} // namespace jkfs
