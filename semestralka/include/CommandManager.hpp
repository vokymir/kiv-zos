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
  std::map<std::string, std::unique_ptr<ICommand>> commands_;

public:
  void register_command(std::unique_ptr<ICommand> command);
  void run_command(const std::string &line);
  bool exit() { return exit_flag_; }
  void set_vocal(const bool is_vocal) { vocal_ = is_vocal; }
  std::string get_all_commands();
};

} // namespace jkfs
