#pragma once

#include <map>
#include <memory>
#include <string>

#include "ICommand.hpp"

namespace jkfs {

class CommandManager {
private:
  bool m_exit_flag = false;
  bool m_vocal = false;
  std::string m_filename = "";
  std::map<std::string, std::unique_ptr<ICommand>> m_commands;

public:
  void registerCommand(std::unique_ptr<ICommand> a_command);
  void runCommand(const std::string &a_line);
  bool exit() { return m_exit_flag; }
  void setVocal(const bool a_is_vocal) { m_vocal = a_is_vocal; }
  void setFilename(const std::string a_filename) { m_filename = a_filename; }
  std::string getFilename() { return m_filename; }
  std::string getAllCommands();
};

} // namespace jkfs
