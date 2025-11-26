#include <iostream>
#include <string>

#include "commands.hpp"

namespace jkfs {

RmdirCommand::RmdirCommand() {
  id_ = "rmdir";
  name_ = "Remove directory";
  desc_ = "Remove directory if exists and is empty.";
  how_ = "rmdir empty_dir";
}

void RmdirCommand::execute(std::vector<std::string> &a_args) noexcept {
  std::string args = "";
  for (auto arg : a_args) {
    args += arg + " ";
  }
  std::cout << "Running " + name_ + " command, with arguments: " << args
            << std::endl;
}

} // namespace jkfs
