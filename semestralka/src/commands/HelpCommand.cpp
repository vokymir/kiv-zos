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

void HelpCommand::execute(std::vector<std::string> &a_args) noexcept {
  std::string args = "";
  for (auto arg : a_args) {
    args += arg + " ";
  }

  std::cout << "All available commands:\n"
            << manager_.get_all_commands() << std::endl;
}

} // namespace jkfs
