#include <iostream>
#include <ostream>
#include <string>

#include "commands.hpp"

namespace jkfs {

ExitCommand::ExitCommand() {
  id_ = "exit";
  name_ = "Exit";
  desc_ = "Leave the filesystem terminal.";
  how_ = "exit // just write this";
}

void ExitCommand::execute([[maybe_unused]] std::vector<std::string> &a_args) {
  std::cout << "Exiting the filesystem terminal now." << std::endl;
}

} // namespace jkfs
