#include <string>

#include "commands.hpp"
#include "errors.hpp"

namespace jkfs {

AddCommand::AddCommand() {
  id_ = "add";
  name_ = "Add";
  desc_ = "Append contents of one file to the end of the other.";
  how_ = "add s1 s2 // append s2 to the end of file s1";
}

void AddCommand::execute_inner(const std::vector<std::string> &args) {
  if (args.size() < 2) {
    throw command_error(
        "Add command requires 2 arguments. See 'add -h' for more info.");
  }

  auto from_id_path = fs_.path_lookup(args[0]);
  if (from_id_path.empty()) {
    throw command_error("cannot find source path");
  }
  auto from_id = from_id_path.back();

  auto to_id_path = fs_.path_lookup(args[1]);
  if (to_id_path.empty()) {
    throw command_error("cannot find source path");
  }
  auto to_id = to_id_path.back();

  auto from_data = fs_.file_read(from_id);
  auto to_inode = fs_.inode_read(to_id);

  fs_.file_write(to_id, to_inode.file_size,
                 reinterpret_cast<const char *>(from_data.data()),
                 from_data.size());
}

} // namespace jkfs
