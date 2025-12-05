#include <filesystem>
#include <iostream>
#include <string>

#include "CommandManager.hpp"
#include "commands.hpp"
#include "errors.hpp"

namespace jkfs {

// stores the depth of recursion
unsigned long long LoadCommand::depth_ = 0;

LoadCommand::LoadCommand(CommandManager &manager) : manager_(manager) {
  id_ = "load";
  name_ = "Load and execute_inner";
  desc_ = "Load a file from the drive and execute_inner all instructions "
          "written in it.";
  how_ = "load ./cmds // where cmds is file with instructions";

  success_message_ = "OK";
  failure_message_ = "FILE NOT FOUND";
}

void LoadCommand::execute_inner(const std::vector<std::string> &args) {
  if (args.empty()) {
    throw command_error("Load command need one argument.");
  }

  std::string path(args[0]);
  if (!std::filesystem::exists(path)) {
    throw command_error("File on given path doesn't exist.");
  }

  std::ifstream file(path);
  depth_++;

  for (std::string line = ""; std::getline(file, line);) {
    // show recursion depth
    std::cout << depth_;
    for (auto i = 0; i < depth_; i++) {
      std::cout << ">";
    }
    // show the command
    std::cout << " " << line << std::endl;
    // do the command
    manager_.run_command(line);
  }

  depth_--;
}

} // namespace jkfs
