#pragma once

#include <map>
#include <memory>
#include <string>

#include "ICommand.hpp"

namespace jkfs {

class CommandManager {
private:
    bool m_exit_flag = false;
    std::string m_filename = "";
    std::map<std::string, std::unique_ptr<ICommand>> m_commands;
public:
    void registerCommand(std::unique_ptr<ICommand> a_command);
    void runCommand(const std::string& a_line);
    bool exit() { return m_exit_flag; }
    void set_filename(const std::string a_filename) { m_filename = a_filename; }
    std::string get_filename() { return m_filename; }
};

}
