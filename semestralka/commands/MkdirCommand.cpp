#include <iostream>
#include <string>

#include "commands.hpp"

namespace jkfs {

MkdirCommand::MkdirCommand() {
  m_id = "mkdir";
  m_name = "Make directory";
  m_desc = "Make new direcory if possible.";
  m_how = "mkdir new-directory";
}

void MkdirCommand::execute(std::vector<std::string> &a_args) {
  std::string args = "";
  for (auto arg : a_args) {
    args += arg + " ";
  }
  std::cout << "Running " + m_name + " command, with arguments: " << args
            << std::endl;
}

} // namespace jkfs
