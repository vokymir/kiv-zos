#pragma once

#include <map>
#include <memory>
#include <string>

#include "ICommand.hpp"

namespace jkfs {

class CommandManager {
private:
    std::map<std::string, std::unique_ptr<ICommand>> m_commands;
public:
    void registerCommand(std::unique_ptr<ICommand> a_command);
    void runCommand(std::string& a_line);
};

}
