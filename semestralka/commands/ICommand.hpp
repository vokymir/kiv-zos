#pragma once

#include <iostream>
#include <string>
#include <vector>

namespace jkfs {

class ICommand {
protected:
    // What user write into the command-line interface.
    std::string m_id = "ic";
    // Human readable somewhat descriptive name.
    std::string m_name = "ICommand";
    // Deeper and more comprehensive description of what the command does.
    std::string m_desc = "Doesn't do anything, is just an interface.";
    // How to use this command.
    std::string m_how = "This command cannot be invoked from the command line.";
    // Examples.
    std::vector<std::string> m_exmp{};
public:
    virtual ~ICommand() = default;
    // Given vector of arguments, execute the command.
    // After operation, writes the result or error message.
    virtual void execute(std::vector<std::string>& a_args) = 0;
    std::string name() { return m_name; }
    std::string id() { return m_id; }
    // Get concrete help for using the command.
    std::string help() {
        std::string res = "";
        res += "┌─\n";
        res += "| << " + m_name + " >>\n";
        res += "| " + m_desc + "\n";
        res += "├─\n";
        res += "| Usage: " + m_id + "\n";
        res += "| " + m_how + "\n";

        if (!m_exmp.empty()){
            res += "├─\n";
            res += "| Examples:\n";

            for (auto example : m_exmp){
                res += "| " + example + "\n";
            }
        }

        res += "└─\n";
        return res;
    }
    void print_help() { std::cout << help() << std::flush; }
};

}
