#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "CommandManager.hpp"
#include "commands.hpp"

void setup_cmds(jkfs::CommandManager& a_manager);
void terminal(jkfs::CommandManager& a_manager, std::string& a_filename);

int main(int argc, char* argv[]){
    std::vector<std::string> args{argv + 1,argv + argc};
    jkfs::CommandManager manager = jkfs::CommandManager();
    std::string filename = "";

    if (argc >= 2){
        filename = args.at(0); // 0 because in args is one item less (no executable path)
    }

    setup_cmds(manager);

    terminal(manager, filename);

    return  EXIT_SUCCESS;
}

// Register all commands to the Command Manager.
// Set managers vocal level.
void setup_cmds(jkfs::CommandManager& a_manager){
    a_manager.set_vocal(false);
    a_manager.registerCommand(std::make_unique<jkfs::CpCommand>());
    a_manager.registerCommand(std::make_unique<jkfs::MvCommand>());
    a_manager.registerCommand(std::make_unique<jkfs::RmCommand>());
    a_manager.registerCommand(std::make_unique<jkfs::MkdirCommand>());
    a_manager.registerCommand(std::make_unique<jkfs::RmdirCommand>());
    a_manager.registerCommand(std::make_unique<jkfs::LsCommand>());
    a_manager.registerCommand(std::make_unique<jkfs::CatCommand>());
    a_manager.registerCommand(std::make_unique<jkfs::CdCommand>());
    a_manager.registerCommand(std::make_unique<jkfs::PwdCommand>());
    a_manager.registerCommand(std::make_unique<jkfs::InfoCommand>());
    a_manager.registerCommand(std::make_unique<jkfs::IncpCommand>());
    a_manager.registerCommand(std::make_unique<jkfs::OutcpCommand>());
    a_manager.registerCommand(std::make_unique<jkfs::LoadCommand>());
    a_manager.registerCommand(std::make_unique<jkfs::FormatCommand>());
    a_manager.registerCommand(std::make_unique<jkfs::ExitCommand>());
    a_manager.registerCommand(std::make_unique<jkfs::StatfsCommand>());
    a_manager.registerCommand(std::make_unique<jkfs::XcpCommand>());
    a_manager.registerCommand(std::make_unique<jkfs::AddCommand>());
}

// Start and run the terminal.
void terminal(jkfs::CommandManager& a_manager, std::string& a_filename){
    bool exit = false;
    std::string line;

    std::cout << "Welcome in the filesystem. Enter commands. When entering -h along with the command, you'll get help." << std::endl;
    a_manager.set_filename(a_filename);

    while (!exit) {
        std::cout << "> ";
        std::getline(std::cin, line);
        a_manager.runCommand(line);
        exit = a_manager.exit();
    }

    std::cout << "Thanks for using the filesystem, see you later..." << std::endl;
}
