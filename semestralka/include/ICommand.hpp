#pragma once

#include "filesystem.hpp"
#include <iostream>
#include <string>
#include <vector>

namespace jkfs {

class ICommand {
protected:
  Filesystem &fs_ = Filesystem::instance();

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

  // standard success/fail messages
  std::string success_message_ = "OK";
  std::string failure_message_ = "FAILURE";
  static constexpr bool SUCCESS = true;
  static constexpr bool FAILURE = false;
  // print standard success/failure message
  void inline print_message(bool success) const noexcept {
    std::cout << (success ? success_message_ : failure_message_) << std::endl;
  }

public:
  virtual ~ICommand() = default;
  // Given vector of arguments, execute the command.
  // After operation, writes the result or error message to stdout/stderr
  virtual void execute(std::vector<std::string> &args) noexcept = 0;

  std::string name() const noexcept { return name_; }
  std::string id() const noexcept { return id_; }

  // Get concrete help for using the command.
  std::string help() const noexcept {
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
  void print_help() const noexcept { std::cout << help() << std::flush; }
};

} // namespace jkfs
