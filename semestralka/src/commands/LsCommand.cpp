#include <iostream>
#include <string>

#include "commands.hpp"

namespace jkfs {

LsCommand::LsCommand() {
  id_ = "ls";
  name_ = "List directory";
  desc_ = "List all items in directory.";
  how_ = "ls . // lists all items in current directory";
}

void LsCommand::execute_inner([[maybe_unused]] std::vector<std::string> &_) {
  auto cwd = fs_.path_lookup(".");
  auto items = fs_.dir_list(cwd);

  for (const auto &item : items) {
    std::cout << item.item_name.data() << " ";
  }
  std::cout << std::endl;
}

} // namespace jkfs
