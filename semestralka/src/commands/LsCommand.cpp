#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>

#include "commands.hpp"
#include "errors.hpp"

namespace jkfs {

LsCommand::LsCommand() {
  id_ = "ls";
  name_ = "List directory";
  desc_ = "List all items in directory.";
  how_ = "ls [path] [options]";
  exmp_ = {"ls // cwd",
           "ls . // cwd",
           "ls subdir // subdir",
           "ls / // root",
           "ls filename // will just print filename",
           "ls -l // will list with more info and in list",
           "ls -a // will show even hidden files",
           "ls -la // combination of options",
           "ls subdir -la // combination of path & options"};

  success_message_ = "";
  failure_message_ = "PATH NOT FOUND (neexistujici adresar)";
}

void LsCommand::execute_inner(const std::vector<std::string> &args) {
  std::string path;
  if (args.size() > 0) {
    path = args[0];
  }

  auto flags = get_flags(args);

  auto cwd_path = fs_.path_lookup(path);
  if (cwd_path.empty()) {
    throw command_error("empty cwd path");
  }
  auto cwd = cwd_path.back();

  // mimicking linux behaviour on "ls file"
  auto cwd_inode = fs_.inode_read(cwd);
  if (!cwd_inode.is_dir) {
    std::cout << path << std::endl;
    return;
  }

  auto items = fs_.dir_list(cwd);
  std::sort(items.begin(), items.end());

  for (const auto &item : items) {
    if (!flags.all) { // show all
      if (item.item_name[0] == '.') {
        continue;
      }
    }

    std::ostringstream str;
    auto inode = fs_.inode_read(item.inode);

    str << item.item_name.data();
    if (flags.list) {
      str << (inode.is_dir ? " DIR " : " FILE ") << inode.file_size << "b "
          << inode.node_id << std::endl;
    } else {
      str << " ";
    }

    std::cout << str.str();
  }

  std::cout << std::endl;
}

Ls_Flags LsCommand::get_flags(const std::vector<std::string> &args) {
  Ls_Flags flags;

  // for every argument
  for (const auto &arg : args) {
    if (arg.empty() || arg[0] != '-' || arg == args[0]) {
      continue;
    }
    // for every char
    for (const auto ch : arg) {
      switch (ch) {
      case 'l':
        flags.list = true;
        break;
      case 'a':
        flags.all = true;
        break;
      }
    }
  }

  return flags;
}

std::string LsCommand::get_path(const std::vector<std::string> &args) {
  if (args.empty()) {
    return "";
  }

  // if first argument is not flags
  if (args[0].data()[0] != '-') {
    return args[0];
  }

  return "";
}

} // namespace jkfs
