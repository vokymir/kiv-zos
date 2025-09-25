#include <iostream>
#include <string>

#include "commands.hpp"

namespace jkfs {

MvCommand::MvCommand() {
    m_id = "mv";
    m_name = "Move";
    m_desc = "Move one file to another location. Can be also used for renaming.";
    m_how = "mv s1 s2 // where s1 is source and s2 target location";
}

void MvCommand::execute(std::vector<std::string>& a_args){
    std::string args = "";
    for (auto arg : a_args){args += arg + " ";}
    std::cout << "Running " + m_name + " command, with arguments: " << args << std::endl;
}


}
