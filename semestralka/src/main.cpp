#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "CommandManager.hpp"
#include "commands.hpp"

using jkfs::Filesystem;

void setup_cmds(jkfs::CommandManager &manager);
void terminal(jkfs::CommandManager &manager);

int main(int argc, char *argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  jkfs::CommandManager manager = jkfs::CommandManager();
  std::string filename = "";

  if (argc >= 2) {
    filename =
        args.at(0); // 0 because in args is one item less (no executable path)
  }

  // setup fs
  Filesystem::instance(filename);

  setup_cmds(manager);

  terminal(manager);

  return EXIT_SUCCESS;
}

// Register all commands to the Command Manager.
// Set managers vocal level.
void setup_cmds(jkfs::CommandManager &manager) {
  manager.set_vocal(false);
  manager.register_command(std::make_unique<jkfs::CpCommand>());
  manager.register_command(std::make_unique<jkfs::MvCommand>());
  manager.register_command(std::make_unique<jkfs::RmCommand>());
  manager.register_command(std::make_unique<jkfs::MkdirCommand>());
  manager.register_command(std::make_unique<jkfs::RmdirCommand>());
  manager.register_command(std::make_unique<jkfs::LsCommand>());
  manager.register_command(std::make_unique<jkfs::CatCommand>());
  manager.register_command(std::make_unique<jkfs::CdCommand>());
  manager.register_command(std::make_unique<jkfs::PwdCommand>());
  manager.register_command(std::make_unique<jkfs::InfoCommand>());
  manager.register_command(std::make_unique<jkfs::IncpCommand>());
  manager.register_command(std::make_unique<jkfs::OutcpCommand>());
  manager.register_command(std::make_unique<jkfs::LoadCommand>());
  manager.register_command(std::make_unique<jkfs::FormatCommand>());
  manager.register_command(std::make_unique<jkfs::ExitCommand>());
  manager.register_command(std::make_unique<jkfs::StatfsCommand>());
  manager.register_command(std::make_unique<jkfs::XcpCommand>());
  manager.register_command(std::make_unique<jkfs::AddCommand>());
  manager.register_command(std::make_unique<jkfs::HelpCommand>(manager));
}

// Start and run the terminal.
void terminal(jkfs::CommandManager &manager) {
  bool exit = false;
  std::string line;

  std::cout << "Welcome to the filesystem. Enter any command to continue.\n\
When entered -h after any command, help for the command will be shown.\n\
All available commands (see 'help' command):\n" +
                   manager.get_all_commands()
            << std::endl;

  while (!exit) {
    std::cout << "> ";
    std::getline(std::cin, line);
    manager.run_command(line);
    exit = manager.exit();
  }

  std::cout << "Thanks for using the filesystem, see you later..." << std::endl;
}
