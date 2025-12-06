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

  auto p = fs_.path_lookup(args[0]);
  if (!p.empty()) {
    failure_message_ = "EXIST (nelze založit, již existuje)";
    throw command_error("The target path is already occupied");
  }

  fs_.dir_create_recursive(args[0]);
}

} // namespace jkfs
