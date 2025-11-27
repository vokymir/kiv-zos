#include <filesystem>
#include <string>

#include "CommandManager.hpp"
#include "commands.hpp"

namespace jkfs {

LoadCommand::LoadCommand(CommandManager &manager) : manager_(manager) {
  id_ = "load";
  name_ = "Load and execute";
  desc_ =
      "Load a file from the drive and execute all instructions written in it.";
  how_ = "load ./cmds // where cmds is file with instructions";
}

void LoadCommand::execute(std::vector<std::string> &args) noexcept {
  if (args.empty()) {
    std::cout << "NEEDS ARGUMENT" << std::endl;
    return;
  }

  std::string path(args[0]);
  if (!std::filesystem::exists(path)) {
    std::cout << "FILE NOT FOUND" << std::endl;
    return;
  }

  std::ifstream file(path);
  for (std::string line = ""; std::getline(file, line);) {
    std::cout << ">> " << line << std::endl;
    manager_.run_command(line);
  }
}

} // namespace jkfs
