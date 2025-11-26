#include <iostream>
#include <string>

#include "commands.hpp"

namespace jkfs {

LoadCommand::LoadCommand() {
  m_id = "load";
  m_name = "Load and execute";
  m_desc =
      "Load a file from the drive and execute all instructions written in it.";
  m_how = "load ./cmds // where cmds is file with instructions";
}

void LoadCommand::execute(std::vector<std::string> &a_args) {
  std::string args = "";
  for (auto arg : a_args) {
    args += arg + " ";
  }
  std::cout << "Running " + m_name + " command, with arguments: " << args
            << std::endl;
}

} // namespace jkfs
