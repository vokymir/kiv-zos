#include <exception>
#include <filesystem>
#include <string>

#include "CommandManager.hpp"
#include "commands.hpp"
#include "errors.hpp"

namespace jkfs {

LoadCommand::LoadCommand(CommandManager &manager) : manager_(manager) {
  id_ = "load";
  name_ = "Load and execute";
  desc_ =
      "Load a file from the drive and execute all instructions written in it.";
  how_ = "load ./cmds // where cmds is file with instructions";

  success_message_ = "OK";
  failure_message_ = "FILE NOT FOUND";
}

void LoadCommand::execute(std::vector<std::string> &args) noexcept {
  try {
    if (args.empty()) {
      throw command_error("Load command need one argument.");
    }

    std::string path(args[0]);
    if (!std::filesystem::exists(path)) {
      throw command_error("File on given path doesn't exist.");
    }

    std::ifstream file(path);
    for (std::string line = ""; std::getline(file, line);) {
      std::cout << ">> " << line << std::endl;
      manager_.run_command(line);
    }

    print_message(SUCCESS);

  } catch (std::exception &ex) {
    print_message(FAILURE);
  }
}

} // namespace jkfs
