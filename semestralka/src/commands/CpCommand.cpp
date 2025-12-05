#include <filesystem>
#include <iostream>
#include <string>

#include "commands.hpp"
#include "errors.hpp"

namespace jkfs {

CpCommand::CpCommand() {
  id_ = "cp";
  name_ = "Copy";
  desc_ = "Copy one file to another place.";
  how_ = "cp s1 s2 // where s1 is source and s2 target location";
  exmp_ = {"cp s1 s2", "cp s2 s2"};
}

void CpCommand::execute_inner(const std::vector<std::string> &args) {
  if (args.size() < 2) {
    failure_message_ = "FILE NOT FOUND (neni zdroj)";
    throw command_error("The cp command require at least 2 arguments.");
  }

  auto source = fs_.path_lookup(args[0]);
  if (source < 0) {
    failure_message_ = "FILE NOT FOUND (neni zdroj)";
    throw command_error("cannot find source file");
  }
  auto data = fs_.file_read(source);

  // get parent for create/delete
  std::filesystem::path path(args[1]);
  auto parent = fs_.path_lookup(path.parent_path());
  if (parent < 0) {
    failure_message_ = "PATH NOT FOUND (neexistuje cilova cesta)";
    throw command_error("cannot find parent of target path");
  }

  auto target = fs_.path_lookup(args[1]);

  // something is on target path
  if (target >= 0 && has_force_flag(args)) {
    // have permission to kill
    fs_.file_delete(parent, path.filename());
  } else {
    if (fs_.vocal()) {
      std::cout << "The target file already exists. If you wish to "
                   "overwrite it, repeat command with the additional flag "
                   "-f. See more info: 'cp -h'";
    }
    failure_message_ = "PATH NOT FOUND (obsazena cilova cesta)";
    throw command_error("the target path is already used");
  }

  target = fs_.file_create(parent, path.filename());
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
