#pragma once

#include "CommandManager.hpp"
#include "ICommand.hpp"
#include <cstdint>
#include <string>
#include <vector>

namespace jkfs {

class CpCommand : public ICommand { // 1
protected:
  void execute_inner(const std::vector<std::string> &args) override;

public:
  CpCommand();
};

class MvCommand : public ICommand { // 2
protected:
  void execute_inner(const std::vector<std::string> &args) override;

public:
  MvCommand();
};

class RmCommand : public ICommand { // 3
protected:
  void execute_inner(const std::vector<std::string> &args) override;

public:
  RmCommand();
};

class MkdirCommand : public ICommand { // 4
protected:
  void execute_inner(const std::vector<std::string> &args) override;

public:
  MkdirCommand();
};

class RmdirCommand : public ICommand { // 5
protected:
  void execute_inner(const std::vector<std::string> &args) override;

public:
  RmdirCommand();
};

class LsCommand : public ICommand { // 6
protected:
  void execute_inner(const std::vector<std::string> &args) override;

public:
  LsCommand();
};

class CatCommand : public ICommand { // 7
protected:
  void execute_inner(const std::vector<std::string> &args) override;

public:
  CatCommand();
};

class CdCommand : public ICommand { // 8
protected:
  void execute_inner(const std::vector<std::string> &args) override;

public:
  CdCommand();
};

class PwdCommand : public ICommand { // 9
protected:
  void execute_inner(const std::vector<std::string> &args) override;

public:
  PwdCommand();
};

class InfoCommand : public ICommand { // 10
protected:
  void execute_inner(const std::vector<std::string> &args) override;

public:
  InfoCommand();
};

class IncpCommand : public ICommand { // 11
protected:
  void execute_inner(const std::vector<std::string> &args) override;

private:
  std::vector<uint8_t> read_real_file(const std::string &path);
  void write_unreal_file(std::string &path, std::vector<uint8_t> &input);

public:
  IncpCommand();
};

class OutcpCommand : public ICommand { // 12
protected:
  void execute_inner(const std::vector<std::string> &args) override;

private:
  std::vector<uint8_t> read_unreal_file(const std::string &path);
  void write_real_file(const std::string &path, std::vector<uint8_t> &input);

public:
  OutcpCommand();
};

class LoadCommand : public ICommand { // 13
protected:
  void execute_inner(const std::vector<std::string> &args) override;

private:
  CommandManager &manager_;

public:
  LoadCommand(CommandManager &manager);
};

class FormatCommand : public ICommand { // 14
protected:
  void execute_inner(const std::vector<std::string> &args) override;

public:
  FormatCommand();
  // expects 1 argument, size of filesystem
};

class ExitCommand : public ICommand { // 15
protected:
  void execute_inner(const std::vector<std::string> &args) override;

public:
  ExitCommand();
};

class StatfsCommand : public ICommand { // 16
protected:
  void execute_inner(const std::vector<std::string> &args) override;

public:
  StatfsCommand();
};

class XcpCommand : public ICommand { // BONUS
protected:
  void execute_inner(const std::vector<std::string> &args) override;

public:
  XcpCommand();
};

class AddCommand : public ICommand { // BONUS
protected:
  void execute_inner(const std::vector<std::string> &args) override;

public:
  AddCommand();
};

class HelpCommand : public ICommand { // USEFUL ADDITION
protected:
  void execute_inner(const std::vector<std::string> &args) override;

private:
  CommandManager &manager_;

public:
  HelpCommand(CommandManager &manager);
};

} // namespace jkfs
