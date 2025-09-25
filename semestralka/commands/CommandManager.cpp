#include <iostream>
#include <sstream>

#include "CommandManager.hpp"

namespace jkfs {

// Add to available commands, if the name is not already registered.
void CommandManager::registerCommand(std::unique_ptr<ICommand> a_command){
    auto result = m_commands.insert({a_command->id(), std::move(a_command)});
    if (!result.second) {
        std::cout << "Command " << a_command->id() << " already registered!\n";
    } else {
        std::cout << "Registered " << result.first->second->name() << " command!\n";
    }
}

// Parse the line from the 'terminal' and run the corresponding command.
// If the commands doesn't exist, warns the user.
void CommandManager::runCommand(std::string& a_line){
    std::istringstream iss(a_line);
    std::string cmdName;
    std::vector<std::string> args;
    std::string arg;
    bool help = false;

    iss >> cmdName;

    while (iss >> arg){
        args.push_back(arg);
        if (arg.contains("-h")){help = true;}
    }

    auto it = m_commands.find(cmdName);
    if (it != m_commands.end()) {
        if (help) { it->second->print_help(); }
        else { it->second->execute(args); }
    } else {
        std::cout << "Unknown command: " << cmdName << std::endl;
    }
}

}
