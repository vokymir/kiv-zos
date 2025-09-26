#include <iostream>
#include <string>

#include "commands.hpp"

namespace jkfs {

FormatCommand::FormatCommand() {
    m_id = "format";
    m_name = "Format filesystem";
    m_desc = "Format the file given as a parameter at the start of the program. Format to the filesystem of given size. If file already existed (no matter if was filesystem or not) it will be overwritten, if didn't exist it will be created.";
    m_how = "format 600MB // format the file to be filesystem of 600 Mebibyte";
}

void FormatCommand::execute(std::vector<std::string>& a_args){
    std::string args = "";
    for (auto arg : a_args){args += arg + " ";}
    std::cout << "Running " + m_name + " command, with arguments: " << args << std::endl;
}


}
