#include <iostream>
#include <string>

#include "commands.hpp"
#include "errors.hpp"

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
  auto inode_id_path = fs_.current_directory();
  if (!args.empty()) {
    inode_id_path = fs_.path_lookup(args[0]);
  }

  if (inode_id_path.empty()) {
    throw command_error("Cannot find specified file/dir.");
  }
  auto inode_id = inode_id_path.back();

  auto inode = fs_.inode_read(inode_id);
  auto clusters = fs_.file_list_clusters(inode_id);

  std::cout << inode << std::endl;

  std::cout << "used clusters:\n 1.data: ";
  print_cluster_ranges(std::get<0>(clusters));
  std::cout << " 2.overhead: ";
  print_cluster_ranges(std::get<1>(clusters));
  std::cout << std::endl;
}

void InfoCommand::print_cluster_ranges(const std::vector<int32_t> &clusters) {
  if (clusters.empty())
    return;

  int32_t start = clusters[0];
  int32_t prev = clusters[0];

  for (size_t i = 1; i <= clusters.size(); ++i) {
    if (i == clusters.size() || clusters[i] != prev + 1) {
      // end of a range
      if (start == prev) {
        std::cout << start << " ";
      } else {
        std::cout << start << "-" << prev << " ";
      }
      if (i < clusters.size()) {
        start = clusters[i];
      }
    }
    if (i < clusters.size()) {
      prev = clusters[i];
    }
  }
  std::cout << std::endl;
}

} // namespace jkfs
