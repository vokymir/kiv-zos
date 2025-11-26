#pragma once

#include <iostream>
#include <string>
#include <vector>

namespace jkfs {

class ICommand {
protected:
  // What user write into the command-line interface.
  std::string id_ = "ic";
  // Human readable somewhat descriptive name.
  std::string name_ = "ICommand";
  // Deeper and more comprehensive description of what the command does.
  std::string desc_ = "Doesn't do anything, is just an interface.";
  // How to use this command.
  std::string how_ = "This command cannot be invoked from the command line.";
  // Examples.
  std::vector<std::string> exmp_{};

public:
  virtual ~ICommand() = default;
  // Given vector of arguments, execute the command.
  // After operation, writes the result or error message.
  virtual void execute(std::vector<std::string> &args) = 0;

  std::string name() { return name_; }
  std::string id() { return id_; }

  // Get concrete help for using the command.
  std::string help() {
    std::string res = "";
    res += "┌─\n";
    res += "| << " + name_ + " >>\n";
    res += "| " + desc_ + "\n";
    res += "├─\n";
    res += "| Usage: " + id_ + "\n";
    res += "| " + how_ + "\n";

    if (!exmp_.empty()) {
      res += "├─\n";
      res += "| Examples:\n";

      for (auto example : exmp_) {
        res += "| " + example + "\n";
      }
    }

    res += "└─\n";
    return res;
  }
  void print_help() { std::cout << help() << std::flush; }
};

} // namespace jkfs
