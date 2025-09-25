#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "CommandManager.hpp"
#include "commands.hpp"

int main(int argc, char* argv[]){
    std::vector<std::string> args{argv + 1,argv + argc};
    std::string arg_line = "";
    jkfs::CommandManager manager = jkfs::CommandManager();

    for (auto arg : args){ arg_line += arg + " ";}

    manager.registerCommand(std::make_unique<jkfs::CpCommand>());

    manager.runCommand(arg_line);

}
