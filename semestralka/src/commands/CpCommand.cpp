#include <filesystem>
#include <iostream>
#include <string>

#include "commands.hpp"
#include "errors.hpp"

namespace jkfs {

CpCommand::CpCommand() {
  id_ = "cp";
  name_ = "Copy";
  desc_ =
      "Copy one file to another place. If on that place is already a file, \n\
| copy won't overwrite it unless a force flag is used: -f or --force";
  how_ = "cp s1 s2 // where s1 is source and s2 target location";
  exmp_ = {"cp s1 s2 // s1 exists, s2 doesn't", "cp s2 s2 // won't work",
           "cp s2 s2 -f // will work", "cp s2 s2 --force // also works"};

  failure_message_ = "FILE NOT FOUND (neni zdroj)";
}

void CpCommand::execute_inner(const std::vector<std::string> &args) {
  if (args.size() < 2) {
    throw command_error("The cp command require at least 2 arguments.");
  }

  auto source_path = fs_.path_lookup(args[0]);
  if (source_path.empty()) {
    throw command_error("cannot find source file");
  }
  auto source = source_path.back();
  auto data = fs_.file_read(source);

  // get parent
  std::filesystem::path path(args[1]);
  auto target_parent_path = fs_.path_lookup(path.parent_path());
  if (target_parent_path.empty()) {
    failure_message_ = "PATH NOT FOUND (neexistuje cilova cesta)";
    throw command_error("cannot find parent of target path");
  }
  int32_t parent = target_parent_path.back();

  auto target_path = fs_.path_lookup(args[1]);

  // something is on target path
  if (!target_path.empty()) {
    if (has_force_flag(args)) {
      // have permission to kill
      fs_.file_delete(parent, path.filename());
    } else {
      if (fs_.vocal()) {
        std::cout << "The target file already exists. If you wish to "
                     "overwrite it, repeat command with the additional flag "
                     "-f. See more info: 'cp -h'"
                  << std::endl;
      }
      failure_message_ = "PATH NOT FOUND (obsazena cilova cesta)";
      throw command_error("the target path is already used");
    }
  }

  // create new file & copy contents
  auto target = fs_.file_create(parent, path.filename());
  fs_.file_write(target, 0, reinterpret_cast<const char *>(data.data()),
                 data.size());
}

bool CpCommand::has_force_flag(const std::vector<std::string> &args) const {
  for (auto i = 2; i < args.size(); i++) {
    if (args[i] == "-f" || args[i] == "--force") {
      return true;
    }
  }
  return false;
}

} // namespace jkfs
