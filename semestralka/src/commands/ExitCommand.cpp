#include <iostream>
#include <ostream>
#include <string>

#include "commands.hpp"

namespace jkfs {

ExitCommand::ExitCommand() {
  id_ = "exit";
  name_ = "Exit";
  desc_ = "Leave the filesystem terminal.";
  how_ = "exit";

  success_message_ = "";
  failure_message_ = "";
}

void ExitCommand::execute_inner(
    [[maybe_unused]] const std::vector<std::string> &_) {
  if (fs_.vocal()) {
    std::cout << "Exiting the filesystem terminal now." << std::endl;
  }
}

} // namespace jkfs
