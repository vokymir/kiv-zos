#include <iostream>
#include <string>

#include "CommandManager.hpp"
#include "commands.hpp"

namespace jkfs {

HelpCommand::HelpCommand(CommandManager &manager) : manager_(manager) {
  id_ = "help";
  name_ = "Help";
  desc_ = "Show all available commands which can be used.";
  how_ = "help";
}

void HelpCommand::execute_inner([[maybe_unused]] std::vector<std::string> &_) {
  std::cout << "All available commands:\n"
            << manager_.get_all_commands() << std::endl;
}

} // namespace jkfs
