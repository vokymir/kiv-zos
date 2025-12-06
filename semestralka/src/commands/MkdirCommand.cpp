#include <string>

#include "commands.hpp"
#include "errors.hpp"

namespace jkfs {

MkdirCommand::MkdirCommand() {
  id_ = "mkdir";
  name_ = "Make directory";
  desc_ = "Make new direcory if possible.";
  how_ = "mkdir new-directory";

  failure_message_ = "PATH NOT FOUND (neexistuje zadana cesta)";
}

void MkdirCommand::execute_inner(const std::vector<std::string> &args) {
  if (args.empty()) {
    throw command_error("The mkdir command requires one argument.");
  }

  fs_.dir_create_recursive(args[0]);
}

} // namespace jkfs
