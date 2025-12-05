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

private:
  bool has_force_flag(const std::vector<std::string> &args) const;

public:
  CpCommand();
};

class MvCommand : public ICommand { // 2
protected:
  void execute_inner(const std::vector<std::string> &args) override;

private:
  bool has_force_flag(const std::vector<std::string> &args) const;

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

private:
  void print_cluster_ranges(const std::vector<int32_t> &clusters);

public:
  InfoCommand();
};

class IncpCommand : public ICommand { // 11
protected:
  void execute_inner(const std::vector<std::string> &args) override;

private:
  std::vector<uint8_t> read_real_file(const std::string &path);
  void write_unreal_file(const std::string &path, std::vector<uint8_t> &input);

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
  static unsigned long long depth_;
  CommandManager &manager_;

public:
  LoadCommand(CommandManager &manager);
};

// File size that the file can be formatted to.
// Only used in FormatCommand
enum class FILE_SIZE {
  B,
  KB,
  MB,
};

class FormatCommand : public ICommand { // 14
protected:
  void execute_inner(const std::vector<std::string> &args) override;

private:
  // from arguments of FORMAT command, calculate the new total size of file
  int get_total_size(const std::string &arg);

  // from string, e.g. 600MB convert to 600 and MB
  std::tuple<int, FILE_SIZE> get_size(const std::string &arg);

  // convert from string to enum, optionally can start parsing string on
  // position greater than 0
  FILE_SIZE convert_filesize(const std::string &str, size_t pos = 0);

  // convert FS to integer
  int convert_size(enum FILE_SIZE fs);

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

private:
  bool has_force_flag(const std::vector<std::string> &args) const;

public:
  XcpCommand();
};

class AddCommand : public ICommand { // BONUS
protected:
  void execute_inner(const std::vector<std::string> &args) override;

public:
  AddCommand();
};

// show all available commands
class HelpCommand : public ICommand { // USEFUL ADDITION
protected:
  void execute_inner(const std::vector<std::string> &args) override;

private:
  CommandManager &manager_;

public:
  HelpCommand(CommandManager &manager);
};

// allows shell commands execution inside FS
class ExecCommand : public ICommand { // USEFUL ADDITION
protected:
  void execute_inner(const std::vector<std::string> &args) override;

public:
  ExecCommand();
};

} // namespace jkfs
