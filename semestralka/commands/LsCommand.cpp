#include <iostream>
#include <string>

#include "commands.hpp"

namespace jkfs {

LsCommand::LsCommand() {
    m_id = "ls";
    m_name = "List directory";
    m_desc = "List all items in directory.";
    m_how = "ls . // lists all items in current directory";
}

void LsCommand::execute(std::vector<std::string>& a_args){
    std::string args = "";
    for (auto arg : a_args){args += arg + " ";}
    std::cout << "Running " + m_name + " command, with arguments: " << args << std::endl;
}


}
