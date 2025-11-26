#include <iostream>
#include <sstream>

#include "CommandManager.hpp"

namespace jkfs {

// Add to available commands, if the name is not already registered.
void CommandManager::registerCommand(std::unique_ptr<ICommand> command) {
  auto result = commands_.insert({command->id(), std::move(command)});
  if (!vocal_) {
    return;
  }

  if (!result.second) {
    std::cout << "Command " << command->id() << " already registered!\n";
  } else {
    std::cout << "Registered " << result.first->second->name() << " command!\n";
  }
}

// Parse the line from the 'terminal' and run the corresponding command.
// If the commands doesn't exist, warns the user.
void CommandManager::runCommand(const std::string &line) {
  std::istringstream iss(line);
  std::string cmdName;
  std::vector<std::string> args;
  std::string arg;
  bool help = false;

  iss >> cmdName;

  while (iss >> arg) {
    args.push_back(arg);
    if (arg.contains("-h")) {
      help = true;
    }
  }

  auto it = commands_.find(cmdName);
  if (it == commands_.end()) {
    std::cout << "Unknown command: " << cmdName << std::endl;
    return;
  }

  if (help) {
    it->second->print_help();
  } else {
    it->second->execute(args);
    if (it->second->id() == "exit") {
      exit_flag_ = true;
    }
  }
}

// Return all available (= registered) commands separated by whitespace.
std::string CommandManager::getAllCommands() {
  std::string res = "";

  for (auto &cmd : commands_) {
    res += cmd.first + " ";
  }

  return res;
}

} // namespace jkfs
