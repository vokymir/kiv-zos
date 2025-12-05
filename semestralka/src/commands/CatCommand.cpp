#include <ios>
#include <iostream>
#include <string>

#include "commands.hpp"
#include "errors.hpp"

namespace jkfs {

CatCommand::CatCommand() {
  id_ = "cat";
  name_ = "Concatenate";
  desc_ = "Print the contents of one file.";
  how_ = "cat file.txt";

  success_message_ = "";
  failure_message_ = "FILE NOT FOUND (neni zdroj)";
}

void CatCommand::execute_inner(const std::vector<std::string> &args) {
  if (args.empty()) {
    throw command_error(
        "Cat command requires one argument - the name of file to concatenate.");
  }

  // find the file contents
  auto path = args[0];
  auto inode = fs_.path_lookup(path).back();

  // if target path is a directory
  // NOTE: it works either way, but this is how linux behaves
  if (fs_.inode_read(inode).is_dir) {
    failure_message_ = "FILE IS A DIRECTORY";
    throw command_error("Cannot cat a directory: " + path);
  }

  auto bytes = fs_.file_read(inode);

  // write bytes to stdout
  std::cout.write(reinterpret_cast<const char *>(bytes.data()),
                  static_cast<std::streamsize>(bytes.size()));
  std::cout << std::endl;
}

} // namespace jkfs
