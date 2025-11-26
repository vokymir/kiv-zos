#include <iostream>
#include <string>

#include "commands.hpp"

namespace jkfs {

MkdirCommand::MkdirCommand() {
  id_ = "mkdir";
  name_ = "Make directory";
  desc_ = "Make new direcory if possible.";
  how_ = "mkdir new-directory";
}

void MkdirCommand::execute(std::vector<std::string> &a_args) noexcept {
  std::string args = "";
  for (auto arg : a_args) {
    args += arg + " ";
  }
  std::cout << "Running " + name_ + " command, with arguments: " << args
            << std::endl;
}

} // namespace jkfs
