#include <algorithm>
#include <iostream>
#include <string>

#include "commands.hpp"

namespace jkfs {

LsCommand::LsCommand() {
  id_ = "ls";
  name_ = "List directory";
  desc_ = "List all items in directory.";
  how_ = "ls // lists all items in current directory";
  exmp_ = {"ls // cwd", "ls . // cwd", "ls subdir // subdir", "ls / // root",
           "ls filename // will just print filename"};
}

void LsCommand::execute_inner(const std::vector<std::string> &args) {
  std::string path;
  if (args.size() > 0) {
    path = args[0];
  }

  auto cwd = fs_.path_lookup(path);
  // mimicking linux behaviour on "ls file"
  auto cwd_inode = fs_.inode_read(cwd);
  if (!cwd_inode.is_dir) {
    std::cout << path << std::endl;
    return;
  }

  auto items = fs_.dir_list(cwd);
  std::sort(items.begin(), items.end());

  for (const auto &item : items) {
    std::cout << item.item_name.data() << " ";
  }
  std::cout << std::endl;
}

} // namespace jkfs
