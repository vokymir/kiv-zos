#include <iostream>
#include <string>

#include "commands.hpp"

namespace jkfs {

XcpCommand::XcpCommand() {
    m_id = "xcp";
    m_name = "Join and Copy";
    m_desc = "Join contents of two files and copy them to target location.";
    m_how = "xcp s1 s2 s3 // where s1, s2 are source and s3 target location";
}

void XcpCommand::execute(std::vector<std::string>& a_args){
    std::string args = "";
    for (auto arg : a_args){args += arg + " ";}
    std::cout << "Running " + m_name + " command, with arguments: " << args << std::endl;
}


}
