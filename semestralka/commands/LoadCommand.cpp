#include <iostream>
#include <string>

#include "commands.hpp"

namespace jkfs {

LoadCommand::LoadCommand() {
  id_ = "load";
  name_ = "Load and execute";
  desc_ =
      "Load a file from the drive and execute all instructions written in it.";
  how_ = "load ./cmds // where cmds is file with instructions";
}

void LoadCommand::execute(std::vector<std::string> &a_args) noexcept {
  std::string args = "";
  for (auto arg : a_args) {
    args += arg + " ";
  }
  std::cout << "Running " + name_ + " command, with arguments: " << args
            << std::endl;
}

} // namespace jkfs
