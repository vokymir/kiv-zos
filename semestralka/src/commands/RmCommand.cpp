#include <filesystem>
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

  auto path = fs_.path_lookup(args[0]);
  if (path.empty()) {
    throw command_error("empty path");
  }

  auto last_inode = fs_.inode_read(path.back());
  if (last_inode.is_dir) {
    throw command_error("Cannot remove directory.");
  }

  std::filesystem::path p(args[0]);
  fs_.file_delete(path.back() - 1, p.filename());
}

} // namespace jkfs
