#include <iostream>
#include <string>

#include "commands.hpp"

namespace jkfs {

InfoCommand::InfoCommand() {
  m_id = "info";
  m_name = "Info";
  m_desc = "Print info about file/directory.";
  m_how = "info f1 // f1 is either file or folder";
  m_exmp = {"info file.txt", "info my_folder"};
}

void InfoCommand::execute(std::vector<std::string> &a_args) {
  std::string args = "";
  for (auto arg : a_args) {
    args += arg + " ";
  }
  std::cout << "Running " + m_name + " command, with arguments: " << args
            << std::endl;
}

} // namespace jkfs
