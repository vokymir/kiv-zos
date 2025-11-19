#include <iostream>
#include <string>

#include "CommandManager.hpp"
#include "commands.hpp"

namespace jkfs {

HelpCommand::HelpCommand(CommandManager &manager) : manager_(manager) {
  m_id = "help";
  m_name = "Help";
  m_desc = "Show all available commands which can be used.";
  m_how = "help";
}

void HelpCommand::execute(std::vector<std::string> &a_args) {
  std::string args = "";
  for (auto arg : a_args) {
    args += arg + " ";
  }

  std::cout << manager_.getAllCommands() << std::endl;
}

} // namespace jkfs
