#include <cstdlib>
#include <string>

#include "commands.hpp"
#include "errors.hpp"

namespace jkfs {

ExecCommand::ExecCommand() {
  id_ = "exec";
  name_ = "Execute outwordly command";
  desc_ = "Execute any command current shell is capable of.";
  how_ = "exec top";
}

void ExecCommand::execute_inner(const std::vector<std::string> &args) {
  if (args.empty()) {
    throw command_error("Exec command requires one argument.");
  }

  std::string command;
  for (const auto &arg : args) {
    if (!command.empty())
      command += " ";
    command += arg;
  }

  int ret = std::system(command.c_str());
  if (ret == -1) {
    throw command_error("Failed to execute command.");
  }
}

} // namespace jkfs
