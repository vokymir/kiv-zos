#pragma once

#include "ICommand.hpp"
#include <string>
#include <vector>

namespace jkfs {


class CpCommand : public ICommand { // 1
public:
    CpCommand();
    void execute(std::vector<std::string>& a_args) override;
};

class MvCommand : public ICommand { // 2
public:
    MvCommand();
    void execute(std::vector<std::string>& a_args) override;
};

class RmCommand : public ICommand { // 3
public:
    RmCommand();
    void execute(std::vector<std::string>& a_args) override;
};

class MkdirCommand : public ICommand { // 4
public:
    MkdirCommand();
    void execute(std::vector<std::string>& a_args) override;
};

class RmdirCommand : public ICommand { // 5
public:
    RmdirCommand();
    void execute(std::vector<std::string>& a_args) override;
};

class LsCommand : public ICommand { // 6
public:
    LsCommand();
    void execute(std::vector<std::string>& a_args) override;
};

class CatCommand : public ICommand { // 7
public:
    CatCommand();
    void execute(std::vector<std::string>& a_args) override;
};

class CdCommand : public ICommand { // 8
public:
    CdCommand();
    void execute(std::vector<std::string>& a_args) override;
};

class PwdCommand : public ICommand { // 9
public:
    PwdCommand();
    void execute(std::vector<std::string>& a_args) override;
};

class InfoCommand : public ICommand { // 10
public:
    InfoCommand();
    void execute(std::vector<std::string>& a_args) override;
};

class IncpCommand : public ICommand { // 11
public:
    IncpCommand();
    void execute(std::vector<std::string>& a_args) override;
};

class OutcpCommand : public ICommand { // 12
public:
    OutcpCommand();
    void execute(std::vector<std::string>& a_args) override;
};

class LoadCommand : public ICommand { // 13
public:
    LoadCommand();
    void execute(std::vector<std::string>& a_args) override;
};

class FormatCommand : public ICommand { // 14
public:
    FormatCommand();
    void execute(std::vector<std::string>& a_args) override;
};

class ExitCommand : public ICommand { // 15
public:
    ExitCommand();
    void execute(std::vector<std::string>& a_args) override;
};

class StatfsCommand : public ICommand { // 16
public:
    StatfsCommand();
    void execute(std::vector<std::string>& a_args) override;
};

class XcpCommand : public ICommand { // BONUS
public:
    XcpCommand();
    void execute(std::vector<std::string>& a_args) override;
};

class AddCommand : public ICommand { // BONUS
public:
    AddCommand();
    void execute(std::vector<std::string>& a_args) override;
};
}
