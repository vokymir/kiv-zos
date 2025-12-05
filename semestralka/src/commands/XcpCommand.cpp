#include <string>

#include "commands.hpp"
#include "errors.hpp"

namespace jkfs {

XcpCommand::XcpCommand() {
  id_ = "xcp";
  name_ = "Join and Copy";
  desc_ = "Join contents of two files and copy them to target location.";
  how_ = "xcp s1 s2 s3 // where s1, s2 are source and s3 target location";

  failure_message_ = "FILE NOT FOUND (neni zdroj)";
}

void XcpCommand::execute_inner(const std::vector<std::string> &args) {
  if (args.size() < 3) {
    throw command_error("The xcp command require at least 3 arguments.");
  }

  // find sources & target
  auto source_1_path = fs_.path_lookup(args[0]);
  if (source_1_path.empty()) {
    throw command_error("cannot find source file 1");
  }
  auto source_1 = source_1_path.back();

  auto source_2_path = fs_.path_lookup(args[1]);
  if (source_2_path.empty()) {
    throw command_error("cannot find source file 2");
  }
  auto source_2 = source_2_path.back();

  std::filesystem::path target_path(args[2]);
  auto target_parent_path = fs_.path_lookup(target_path.parent_path());
  if (target_parent_path.empty()) {
    failure_message_ = "PATH NOT FOUND (neexistuje cilova cesta)";
    throw command_error("cannot find parent of target path");
  }
  auto target_parent = target_parent_path.back();

  auto target_path_i = fs_.path_lookup(args[2]);
  if (target_path_i.empty()) {
    failure_message_ = "PATH NOT FOUND (neexistuje cilova cesta)";
    throw command_error("cannot find target path");
  }
  auto target = target_path_i.back();

  // ready data
  auto data = fs_.file_read(source_1);
  auto data2 = fs_.file_read(source_2);
  data.insert(data.end(), data2.begin(), data2.end());

  // something is on target path
  if (target >= 0) {
    if (has_force_flag(args)) {
      // have permission to kill
      fs_.file_delete(target_parent, target_path.filename());
    } else {
      if (fs_.vocal()) {
        std::cout << "The target file already exists. If you wish to "
                     "overwrite it, repeat command with the additional flag "
                     "-f. See more info: 'xcp -h'"
                  << std::endl;
      }
      failure_message_ = "PATH NOT FOUND (obsazena cilova cesta)";
      throw command_error("the target path is already used");
    }
  }

  // create new file & copy contents
  target = fs_.file_create(target_parent, target_path.filename());
  fs_.file_write(target, 0, reinterpret_cast<const char *>(data.data()),
                 data.size());
}

bool XcpCommand::has_force_flag(const std::vector<std::string> &args) const {
  for (auto i = 2; i < args.size(); i++) {
    if (args[i] == "-f" || args[i] == "--force") {
      return true;
    }
  }
  return false;
}

} // namespace jkfs
