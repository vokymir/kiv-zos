#include <cstdint>
#include <iostream>
#include <string>

#include "commands.hpp"
#include "errors.hpp"
#include "filesystem.hpp"

namespace jkfs {

InfoCommand::InfoCommand() {
  id_ = "info";
  name_ = "Info";
  desc_ = "Print info about file/directory.";
  how_ = "info f1 // f1 is either file or folder";
  exmp_ = {"info file.txt", "info my_folder"};

  success_message_ = "";
  failure_message_ = "FILE NOT FOUND (není zdroj)";
}

void InfoCommand::execute_inner(const std::vector<std::string> &args) {
  int32_t inode_id = fs_.current_directory();
  if (!args.empty()) {
    inode_id = fs_.path_lookup(args[0]);
  }

  if (inode_id < 0) {
    throw command_error("Cannot find specified file/dir.");
  }

  auto inode = fs_.inode_read(inode_id);
  auto clusters = fs_.file_list_clusters(inode_id);

  std::cout << inode << "\nused clusters:\n1.data: ";
  for (const auto &cluster : std::get<0>(clusters)) {
    std::cout << cluster << " ";
  }
  std::cout << "\n2.overhead: ";
  for (const auto &cluster : std::get<1>(clusters)) {
    std::cout << cluster << " ";
  }
  std::cout << std::endl;
}

} // namespace jkfs
