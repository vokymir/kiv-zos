#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "CommandManager.hpp"
#include "commands.hpp"

using jkfs::Filesystem;

void setup_cmds(jkfs::CommandManager &manager);
void terminal(jkfs::CommandManager &manager, std::string &filename);

int main(int argc, char *argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  jkfs::CommandManager manager = jkfs::CommandManager();
  std::string filename = "";

  if (argc >= 2) {
    filename =
        args.at(0); // 0 because in args is one item less (no executable path)
  }

  setup_cmds(manager);

  terminal(manager, filename);

  return EXIT_SUCCESS;
}

// Register all commands to the Command Manager.
// Set managers vocal level.
void setup_cmds(jkfs::CommandManager &manager) {
  manager.setVocal(false);
  manager.registerCommand(std::make_unique<jkfs::CpCommand>());
  manager.registerCommand(std::make_unique<jkfs::MvCommand>());
  manager.registerCommand(std::make_unique<jkfs::RmCommand>());
  manager.registerCommand(std::make_unique<jkfs::MkdirCommand>());
  manager.registerCommand(std::make_unique<jkfs::RmdirCommand>());
  manager.registerCommand(std::make_unique<jkfs::LsCommand>());
  manager.registerCommand(std::make_unique<jkfs::CatCommand>());
  manager.registerCommand(std::make_unique<jkfs::CdCommand>());
  manager.registerCommand(std::make_unique<jkfs::PwdCommand>());
  manager.registerCommand(std::make_unique<jkfs::InfoCommand>());
  manager.registerCommand(std::make_unique<jkfs::IncpCommand>());
  manager.registerCommand(std::make_unique<jkfs::OutcpCommand>());
  manager.registerCommand(std::make_unique<jkfs::LoadCommand>());
  manager.registerCommand(std::make_unique<jkfs::FormatCommand>());
  manager.registerCommand(std::make_unique<jkfs::ExitCommand>());
  manager.registerCommand(std::make_unique<jkfs::StatfsCommand>());
  manager.registerCommand(std::make_unique<jkfs::XcpCommand>());
  manager.registerCommand(std::make_unique<jkfs::AddCommand>());
  manager.registerCommand(std::make_unique<jkfs::HelpCommand>(manager));
}

// Start and run the terminal.
void terminal(jkfs::CommandManager &manager, std::string &filename) {
  bool exit = false;
  std::string line;
  Filesystem::instance(filename);

  std::cout << "Welcome to the filesystem. Enter any command to continue.\n\
When entered -h after any command, help for the command will be shown.\n\
All available commands (see 'help' command):\n" +
                   manager.getAllCommands()
            << std::endl;

  while (!exit) {
    std::cout << "> ";
    std::getline(std::cin, line);
    manager.runCommand(line);
    exit = manager.exit();
  }

  std::cout << "Thanks for using the filesystem, see you later..." << std::endl;
}
