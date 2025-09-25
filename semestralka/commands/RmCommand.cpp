#include <iostream>
#include <string>

#include "commands.hpp"

namespace jkfs {

RmCommand::RmCommand() {
    m_id = "rm";
    m_name = "Remove";
    m_desc = "Remove file if exists.";
    m_how = "rm file.txt";
}

void RmCommand::execute(std::vector<std::string>& a_args){
    std::string args = "";
    for (auto arg : a_args){args += arg + " ";}
    std::cout << "Running " + m_name + " command, with arguments: " << args << std::endl;
}


}
