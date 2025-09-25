#pragma once

#include "ICommand.hpp"
#include <string>
#include <vector>

namespace jkfs {

class CpCommand : public ICommand {
public:
    CpCommand();
    void execute(std::vector<std::string>& a_args) override;
};

class ExitCommand : public ICommand {
public:
    ExitCommand();
    void execute(std::vector<std::string>& a_args) override;
};

}
