#include <iostream>
#include <string>

#include "commands.hpp"

namespace jkfs {

CpCommand::CpCommand() {
    m_id = "cp";
    m_name = "Copy";
    m_desc = "Copy one file to another place.";
    m_how = "cp s1 s2 // where s1 is source and s2 target location";
    m_exmp = {"cp s1 s2", "cp s2 s2"};
}

void CpCommand::execute(std::vector<std::string>& a_args){
    std::string args = "";
    for (auto arg : a_args){args += arg + " ";}
    std::cout << "Running " + m_name + " command, with arguments: " << args << std::endl;
}


}
