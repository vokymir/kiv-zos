#include <string>

#include "commands.hpp"
#include "errors.hpp"
#include "structures.hpp"

namespace jkfs {

RmdirCommand::RmdirCommand() {
  id_ = "rmdir";
  name_ = "Remove directory";
  desc_ = "Remove directory if exists and is empty.";
  how_ = "rmdir empty_dir";

  failure_message_ = "FILE NOT FOUND (neexistující adresář)";
}

void RmdirCommand::execute_inner(const std::vector<std::string> &args) {
  if (args.empty()) {
    throw command_error("rmdir command requires one argument");
  }

  auto path = fs_.path_lookup(args[0]);
  if (path.empty()) {
    throw command_error("invalid path");
  }

  auto last_inode = fs_.inode_read(path.back());
  if (!last_inode.is_dir) {
    throw command_error("Cannot remove file which is not a directory.");
  }

  // every directory
  if (!fs_.dir_empty(last_inode.node_id)) {
    failure_message_ = "NOT EMPTY (adresář obsahuje podadresáře, nebo soubory)";
    throw command_error("Cannot remove: directory not empty.");
  }

  std::filesystem::path p(args[0]);
  fs_.file_delete(path[path.size() - 2], p.filename());
}

} // namespace jkfs
