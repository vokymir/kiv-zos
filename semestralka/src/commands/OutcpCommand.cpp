#include <fstream>
#include <ios>
#include <string>

#include "commands.hpp"
#include "errors.hpp"

namespace jkfs {

// TODO: add correct throws
OutcpCommand::OutcpCommand() {
  id_ = "outcp";
  name_ = "Copy out of the filesystem";
  desc_ = "Copy the file from inside the filesystem to outside.";
  how_ = "outcp inside_path.txt outside_path.txt";
}

void OutcpCommand::execute_inner(const std::vector<std::string> &args) {
  if (args.size() < 2) {
    throw command_error("The outcp command requires two arguments.");
  }

  auto data = read_unreal_file(args[0]);
  write_real_file(args[1], data);
}

std::vector<uint8_t> OutcpCommand::read_unreal_file(const std::string &path) {
  auto inode_id = fs_.path_lookup(path);

  return fs_.file_read(inode_id);
}

void OutcpCommand::write_real_file(const std::string &path,
                                   std::vector<uint8_t> &input) {
  std::ofstream file(path, std::ios::binary);
  if (!file) {
    throw command_error("Cannot write into output file.");
  }

  file.seekp(0, std::ios::beg);
  file.write(reinterpret_cast<const char *>(input.data()),
             static_cast<std::streamsize>(input.size()));

  file.close();
}

} // namespace jkfs
