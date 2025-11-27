#pragma once

#include "CommandManager.hpp"
#include "ICommand.hpp"
#include <string>
#include <vector>

namespace jkfs {

class CpCommand : public ICommand { // 1
public:
  CpCommand();
  void execute(std::vector<std::string> &a_args) noexcept override;
};

class MvCommand : public ICommand { // 2
public:
  MvCommand();
  void execute(std::vector<std::string> &a_args) noexcept override;
};

class RmCommand : public ICommand { // 3
public:
  RmCommand();
  void execute(std::vector<std::string> &a_args) noexcept override;
};

class MkdirCommand : public ICommand { // 4
public:
  MkdirCommand();
  void execute(std::vector<std::string> &a_args) noexcept override;
};

class RmdirCommand : public ICommand { // 5
public:
  RmdirCommand();
  void execute(std::vector<std::string> &a_args) noexcept override;
};

class LsCommand : public ICommand { // 6
public:
  LsCommand();
  void execute(std::vector<std::string> &a_args) noexcept override;
};

class CatCommand : public ICommand { // 7
public:
  CatCommand();
  void execute(std::vector<std::string> &a_args) noexcept override;
};

class CdCommand : public ICommand { // 8
public:
  CdCommand();
  void execute(std::vector<std::string> &a_args) noexcept override;
};

class PwdCommand : public ICommand { // 9
public:
  PwdCommand();
  void execute(std::vector<std::string> &a_args) noexcept override;
};

class InfoCommand : public ICommand { // 10
public:
  InfoCommand();
  void execute(std::vector<std::string> &a_args) noexcept override;
};

class IncpCommand : public ICommand { // 11
public:
  IncpCommand();
  void execute(std::vector<std::string> &a_args) noexcept override;
};

class OutcpCommand : public ICommand { // 12
public:
  OutcpCommand();
  void execute(std::vector<std::string> &a_args) noexcept override;
};

class LoadCommand : public ICommand { // 13
public:
  LoadCommand();
  void execute(std::vector<std::string> &a_args) noexcept override;
};

class FormatCommand : public ICommand { // 14
public:
  FormatCommand();
  // expects 1 argument, size of filesystem
  void execute(std::vector<std::string> &a_args) noexcept override;
};

class ExitCommand : public ICommand { // 15
public:
  ExitCommand();
  void execute(std::vector<std::string> &a_args) noexcept override;
};

class StatfsCommand : public ICommand { // 16
public:
  StatfsCommand();
  void execute(std::vector<std::string> &a_args) noexcept override;
};

class XcpCommand : public ICommand { // BONUS
public:
  XcpCommand();
  void execute(std::vector<std::string> &a_args) noexcept override;
};

class AddCommand : public ICommand { // BONUS
public:
  AddCommand();
  void execute(std::vector<std::string> &a_args) noexcept override;
};

class HelpCommand : public ICommand { // USEFUL ADDITION
private:
  CommandManager &manager_;

public:
  HelpCommand(CommandManager &manager);
  void execute(std::vector<std::string> &a_args) noexcept override;
};
} // namespace jkfs
