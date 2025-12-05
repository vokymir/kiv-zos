#include <string>

#include "commands.hpp"

namespace jkfs {

CdCommand::CdCommand() {
  id_ = "cd";
  name_ = "Change directory";
  desc_ = "Go to specified directory.";
  how_ = "cd .. // go to parent directory";
}

void CdCommand::execute_inner(const std::vector<std::string> &args) {
  if (args.empty()) {
    // mimic linux behaviour
    fs_.current_directory(fs_.root_id());
    return;
  }

  auto target = fs_.path_lookup(args[0]);
  fs_.current_directory(target);
}

} // namespace jkfs
