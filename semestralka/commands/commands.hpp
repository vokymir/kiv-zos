#pragma once

#include "ICommand.hpp"

namespace jkfs {

class CpCommand : public ICommand {
public:
    CpCommand();
    virtual void execute(std::vector<std::string>& a_args) override;
};

}
