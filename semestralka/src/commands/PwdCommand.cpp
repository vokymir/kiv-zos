#include <algorithm>
#include <iostream>
#include <string>

#include "commands.hpp"
#include "errors.hpp"
#include "structures.hpp"

namespace jkfs {

PwdCommand::PwdCommand() {
  id_ = "pwd";
  name_ = "Print working directory";
  desc_ = "Display the full name of current directory, from the 'root'.";
  how_ = "pwd";
}

void PwdCommand::execute_inner(
    [[maybe_unused]] const std::vector<std::string> &_) {
  auto cwd = fs_.current_directory();

  std::cout << "/";
  for (auto i = 0; i < cwd.size() - 1; i++) {
    auto parent_id = cwd[i];
    auto child_id = cwd[i + 1];

    auto dir_items = fs_.dir_list(parent_id);
    auto it = std::ranges::find_if(
        dir_items.begin(), dir_items.end(),
        [child_id](const dir_item &item) { return item.inode == child_id; });

    if (it == dir_items.end()) {
      throw command_error("Should've found path, but alas, it's unreachable.");
    }
    std::cout << std::string(it->item_name.data());
  }
}

} // namespace jkfs
