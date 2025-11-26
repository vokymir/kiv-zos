#include <iostream>
#include <string>

#include "commands.hpp"

namespace jkfs {

OutcpCommand::OutcpCommand() {
  m_id = "outcp";
  m_name = "Copy out of the filesystem";
  m_desc = "Copy the file from inside the filesystem to outside.";
  m_how = "outcp inside_path.txt outside_path.txt";
}

void OutcpCommand::execute(std::vector<std::string> &a_args) {
  std::string args = "";
  for (auto arg : a_args) {
    args += arg + " ";
  }
  std::cout << "Running " + m_name + " command, with arguments: " << args
            << std::endl;
}

} // namespace jkfs
