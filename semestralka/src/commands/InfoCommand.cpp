#include <iostream>
#include <string>

#include "commands.hpp"

namespace jkfs {

InfoCommand::InfoCommand() {
  id_ = "info";
  name_ = "Info";
  desc_ = "Print info about file/directory.";
  how_ = "info f1 // f1 is either file or folder";
  exmp_ = {"info file.txt", "info my_folder"};
}

void InfoCommand::execute_inner(std::vector<std::string> &a_args) {
  std::string args = "";
  for (auto arg : a_args) {
    args += arg + " ";
  }
  std::cout << "Running " + name_ + " command, with arguments: " << args
            << std::endl;
}

} // namespace jkfs
