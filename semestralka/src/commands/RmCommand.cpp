#include <string>

#include "commands.hpp"
#include "errors.hpp"

namespace jkfs {

RmCommand::RmCommand() {
  id_ = "rm";
  name_ = "Remove";
  desc_ = "Remove file if exists.";
  how_ = "rm file.txt";

  failure_message_ = "FILE NOT FOUND";
}

void RmCommand::execute_inner(const std::vector<std::string> &args) {
  if (args.empty()) {
    throw command_error("rm command requires one argument");
  }

  auto path_str = args[0];
  auto path = fs_.path_lookup(path_str);
  if (path.empty()) {
    throw command_error("empty path");
  }

  auto last_inode = fs_.inode_read(path.back());
  if (last_inode.is_dir) {
    throw command_error("Cannot remove directory.");
  }

  fs_.file_delete(path[path.size() - 2], fs_.path_filename(path_str));
}

} // namespace jkfs
