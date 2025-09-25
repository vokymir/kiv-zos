#include <iostream>
#include <string>

#include "commands.hpp"

namespace jkfs {

CatCommand::CatCommand() {
    m_id = "cat";
    m_name = "Concatenate";
    m_desc = "Print the contents of one file.";
    m_how = "cat file.txt";
}

void CatCommand::execute(std::vector<std::string>& a_args){
    std::string args = "";
    for (auto arg : a_args){args += arg + " ";}
    std::cout << "Running " + m_name + " command, with arguments: " << args << std::endl;
}


}
