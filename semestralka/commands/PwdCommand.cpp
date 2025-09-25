#include <iostream>
#include <string>

#include "commands.hpp"

namespace jkfs {

PwdCommand::PwdCommand() {
    m_id = "pwd";
    m_name = "Print working directory";
    m_desc = "Display the full name of current directory, from the 'root'.";
    m_how = "pwd";
}

void PwdCommand::execute(std::vector<std::string>& a_args){
    std::string args = "";
    for (auto arg : a_args){args += arg + " ";}
    std::cout << "Running " + m_name + " command, with arguments: " << args << std::endl;
}


}
