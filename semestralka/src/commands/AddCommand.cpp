#include <iostream>
#include <string>

#include "commands.hpp"

namespace jkfs {

AddCommand::AddCommand() {
  id_ = "add";
  name_ = "Add";
  desc_ = "Append contents of one file to the end of the other.";
  how_ = "add s1 s2 // append s2 to the end of file s1";
}

void AddCommand::execute(std::vector<std::string> &a_args) noexcept {
  std::string args = "";
  for (auto arg : a_args) {
    args += arg + " ";
  }
  std::cout << "Running " + name_ + " command, with arguments: " << args
            << std::endl;
}

} // namespace jkfs
