#include <iostream>
#include <string>

#include "commands.hpp"

namespace jkfs {

RmdirCommand::RmdirCommand() {
    m_id = "rmdir";
    m_name = "Remove directory";
    m_desc = "Remove directory if exists and is empty.";
    m_how = "rmdir empty_dir";
}

void RmdirCommand::execute(std::vector<std::string>& a_args){
    std::string args = "";
    for (auto arg : a_args){args += arg + " ";}
    std::cout << "Running " + m_name + " command, with arguments: " << args << std::endl;
}


}
