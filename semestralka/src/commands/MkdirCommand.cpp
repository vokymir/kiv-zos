#include <string>

#include "commands.hpp"
#include "errors.hpp"

namespace jkfs {

MkdirCommand::MkdirCommand() {
  id_ = "mkdir";
  name_ = "Make directory";
  desc_ = "Make new direcory if possible.";
  how_ = "mkdir new-directory";
}

void MkdirCommand::execute_inner(const std::vector<std::string> &args) {
  if (args.empty()) {
    throw command_error("The mkdir command requires one argument.");
  }

  fs_.dir_create(fs_.current_directory().back(), args[0]);
}

} // namespace jkfs
