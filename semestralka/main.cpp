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
        filename = args.at(1);
    }

    setup_cmds(manager);

    terminal(manager, filename);

    return  EXIT_SUCCESS;
}

// Register all commands to the Command Manager.
void setup_cmds(jkfs::CommandManager& a_manager){
    a_manager.registerCommand(std::make_unique<jkfs::CpCommand>());
    a_manager.registerCommand(std::make_unique<jkfs::ExitCommand>());
}

// Start and run the terminal.
void terminal(jkfs::CommandManager& a_manager, std::string& a_filename){
    bool exit = false;
    std::string line;

    std::cout << "Vitejte ve filesystemu. Zadavejte prikazy. U kazdeho muzete napsat -h a dostanete napovedu." << std::endl;
    a_manager.set_filename(a_filename);

    while (!exit) {
        std::cout << "> ";
        std::getline(std::cin, line);
        a_manager.runCommand(line);
        exit = a_manager.exit();
    }

    std::cout << "Dekujeme, ze jste pouzivali filesystem. Tesime se na priste..." << std::endl;
}
