#include <iostream>
#include <string>

#include "commands.hpp"

namespace jkfs {

CdCommand::CdCommand() {
  m_id = "cd";
  m_name = "Change directory";
  m_desc = "Go to specified directory.";
  m_how = "cd .. // go to parent directory";
}

void CdCommand::execute(std::vector<std::string> &a_args) {
  std::string args = "";
  for (auto arg : a_args) {
    args += arg + " ";
  }
  std::cout << "Running " + m_name + " command, with arguments: " << args
            << std::endl;
}

} // namespace jkfs
