#pragma once

#include <map>
#include <memory>
#include <string>

#include "ICommand.hpp"

namespace jkfs {

class CommandManager {
private:
  bool exit_flag_ = false;
  bool vocal_ = false;
  std::string filename_ = "";
  std::map<std::string, std::unique_ptr<ICommand>> commands_;

public:
  void registerCommand(std::unique_ptr<ICommand> command);
  void runCommand(const std::string &line);
  bool exit() { return exit_flag_; }
  void setVocal(const bool is_vocal) { vocal_ = is_vocal; }
  void setFilename(const std::string filename) { filename_ = filename; }
  std::string getFilename() { return filename_; }
  std::string getAllCommands();
};

} // namespace jkfs
