#include <iostream>
#include <string>

#include "commands.hpp"

namespace jkfs {

IncpCommand::IncpCommand() {
    m_id = "incp";
    m_name = "Copy into filesystem";
    m_desc = "Copy the file from outside to this filesystem.";
    m_how = "incp outside_path.txt inside_path.txt";
}

void IncpCommand::execute(std::vector<std::string>& a_args){
    std::string args = "";
    for (auto arg : a_args){args += arg + " ";}
    std::cout << "Running " + m_name + " command, with arguments: " << args << std::endl;
}


}
