#include <iostream>
#include <string>

#include "commands.hpp"
#include "errors.hpp"

namespace jkfs {

MvCommand::MvCommand() {
  id_ = "mv";
  name_ = "Move";
  desc_ = "Move one file to another location.\n\
| Can be also used for renaming.";
  how_ = "mv s1 s2 // where s1 is source and s2 target location";
  exmp_ = {"mv s1 ./test/s2 // s1 exists, test/s2 doesn't",
           "mv s2 s2 // won't work", "mv s2 s2 -f // will work",
           "mv s2 s2 --force // also works",
           "mv file1 novy_file // can use for file renaming"};
}

void MvCommand::execute_inner(const std::vector<std::string> &args) {
  if (args.size() < 2) {
    failure_message_ = "FILE NOT FOUND (neni zdroj)";
    throw command_error("The mv command require at least 2 arguments.");
  }

  std::filesystem::path source_path(args[0]);
  auto source = fs_.path_lookup(source_path).back();
  if (source < 0) {
    failure_message_ = "FILE NOT FOUND (neni zdroj)";
    throw command_error("cannot find source file");
  } else if (source == 0) {
    failure_message_ = "FILE NOT FOUND (neni zdroj)";
    throw command_error("cannot move ROOT DIRECTORY :=)");
  }
  auto data = fs_.file_read(source);

  // get parent for TARGET create/delete
  std::filesystem::path target_path(args[1]);
  auto target_parent = fs_.path_lookup(target_path.parent_path()).back();
  if (target_parent < 0) {
    failure_message_ = "PATH NOT FOUND (neexistuje cilova cesta)";
    throw command_error("cannot find parent of target path");
  }

  auto target = fs_.path_lookup(args[1]).back();

  // something is on target path
  if (target >= 0) {
    if (has_force_flag(args)) {
      // have permission to kill
      fs_.file_delete(target_parent, target_path.filename());
    } else {
      if (fs_.vocal()) {
        std::cout << "The target file already exists. If you wish to "
                     "remove it, repeat command with the additional flag "
                     "-f. See more info: 'mv -h'"
                  << std::endl;
      }
      failure_message_ = "PATH NOT FOUND (obsazena cilova cesta)";
      throw command_error("the target path is already used");
    }
  }

  // remove directory entry & add new directory entry
  auto source_parent = fs_.path_lookup(source_path.parent_path()).back();

  fs_.dir_item_add(target_parent, source, target_path.filename());
  fs_.dir_item_remove(source_parent, source_path.filename());
}

bool MvCommand::has_force_flag(const std::vector<std::string> &args) const {
  for (auto i = 2; i < args.size(); i++) {
    if (args[i] == "-f" || args[i] == "--force") {
      return true;
    }
  }
  return false;
}

} // namespace jkfs
