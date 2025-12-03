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
}

void CatCommand::execute_inner(std::vector<std::string> &args) {
  if (args.empty()) {
    throw command_error(
        "Cat command requires one argument - the name of file to concatenate.");
  }

  // find the file contents
  auto path = args[0];
  auto inode = fs_.path_lookup(path);
  auto bytes = fs_.file_read(inode);

  // write bytes to stdout
  std::cout.write(reinterpret_cast<const char *>(bytes.data()),
                  static_cast<std::streamsize>(bytes.size()));
  std::cout << std::endl;
}

} // namespace jkfs
