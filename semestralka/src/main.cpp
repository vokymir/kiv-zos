#include <cstdlib>
#include <iostream>
#include <memory>
#include <ranges>
#include <readline/history.h>
#include <readline/readline.h>
#include <string>
#include <vector>

#include "CommandManager.hpp"
#include "commands.hpp"

using jkfs::Filesystem;

bool get_vocal(std::vector<std::string> args);
void setup_cmds(jkfs::CommandManager &manager);
void terminal(jkfs::CommandManager &manager);

int main(int argc, char *argv[]) {
  // get args
  auto args =
      std::vector<std::string>(std::views::counted(argv, argc) |
                               std::views::drop(1) | // drop program name
                               std::ranges::to<std::vector<std::string>>());

  jkfs::CommandManager manager = jkfs::CommandManager();
  std::string filename = "";

  if (argc >= 2) {
    filename =
        args.at(0); // 0 because in args is one item less (no executable path)
  }

  // setup fs filename & vocality
  Filesystem::instance(filename).vocal(get_vocal(args));

  setup_cmds(manager);

  terminal(manager);

  return EXIT_SUCCESS;
}

bool get_vocal(std::vector<std::string> args) {
  for (auto &arg : args) {
    if (arg == "-v" || arg == "--vocal") {
      return true;
    }
  }
  return false;
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
  manager.register_command(std::make_unique<jkfs::LoadCommand>(manager));
  manager.register_command(std::make_unique<jkfs::FormatCommand>());
  manager.register_command(std::make_unique<jkfs::ExitCommand>());
  manager.register_command(std::make_unique<jkfs::StatfsCommand>());
  manager.register_command(std::make_unique<jkfs::XcpCommand>());
  manager.register_command(std::make_unique<jkfs::AddCommand>());
  manager.register_command(std::make_unique<jkfs::HelpCommand>(manager));
  manager.register_command(std::make_unique<jkfs::ExecCommand>());
}

// Start and run the terminal.
void terminal(jkfs::CommandManager &manager) {
  std::cout << "Welcome to the filesystem. Enter any command to continue.\n\
When entered -h after any command, help for the command will be shown.\n\
All available commands (see 'help' command):\n" +
                   manager.get_all_commands()
            << std::endl;

  bool exit = false;
  while (!exit) {
    char *input = readline("> ");

    if (!input) {
      break;
    }

    if (*input) {
      add_history(input);
      manager.run_command(input);
    }

    exit = manager.exit();
    free(input);
  }

  std::cout << "Thanks for using the filesystem, see you later..." << std::endl;
}
