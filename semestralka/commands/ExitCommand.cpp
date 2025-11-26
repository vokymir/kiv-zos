#include <iostream>
#include <ostream>
#include <string>

#include "commands.hpp"

namespace jkfs {

ExitCommand::ExitCommand() {
  m_id = "exit";
  m_name = "Exit";
  m_desc = "Leave the filesystem terminal.";
  m_how = "exit // just write this";
}

void ExitCommand::execute([[maybe_unused]] std::vector<std::string> &a_args) {
  std::cout << "Exiting the filesystem terminal now." << std::endl;
}

} // namespace jkfs
