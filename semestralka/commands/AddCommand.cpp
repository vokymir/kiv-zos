#include <iostream>
#include <string>

#include "commands.hpp"

namespace jkfs {

AddCommand::AddCommand() {
  m_id = "add";
  m_name = "Add";
  m_desc = "Append contents of one file to the end of the other.";
  m_how = "add s1 s2 // append s2 to the end of file s1";
}

void AddCommand::execute(std::vector<std::string> &a_args) {
  std::string args = "";
  for (auto arg : a_args) {
    args += arg + " ";
  }
  std::cout << "Running " + m_name + " command, with arguments: " << args
            << std::endl;
}

} // namespace jkfs
