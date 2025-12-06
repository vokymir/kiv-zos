#include <cstddef>
#include <cstdint>
#include <fstream>
#include <ios>
#include <string>
#include <vector>

#include "commands.hpp"
#include "errors.hpp"

namespace jkfs {

IncpCommand::IncpCommand() {
  id_ = "incp";
  name_ = "Copy into filesystem";
  desc_ = "Copy the file from outside to this filesystem.";
  how_ = "incp outside_path.txt inside_path.txt";
}

void IncpCommand::execute_inner(const std::vector<std::string> &args) {
  if (args.size() < 2) {
    throw command_error("The incp command expects two arguments.");
  }

  auto input = read_real_file(args[0]);
  write_unreal_file(args[1], input);
}

std::vector<uint8_t> IncpCommand::read_real_file(const std::string &path) {
  // open file
  std::ifstream file(path, std::ios::binary);
  if (!file) {
    failure_message_ = "FILE NOT FOUND (není zdroj)";
    throw command_error("Cannot read the input file.");
  }

  // get filesize
  file.seekg(0, std::ios::end);
  size_t size = static_cast<size_t>(file.tellg());
  file.seekg(0, std::ios::beg);

  // read into buffer
  std::vector<uint8_t> buffer(size);
  file.read(reinterpret_cast<char *>(buffer.data()),
            static_cast<std::streamsize>(size));

  // close file
  file.close();

  // return
  return buffer;
}

void IncpCommand::write_unreal_file(const std::string &string_path,
                                    std::vector<uint8_t> &input) {
  std::string path(string_path);

  auto path_inodes = fs_.path_lookup(string_path);
  if (!path_inodes.empty()) {
    failure_message_ = "PATH NOT FOUND (obsazena)";
    throw command_error("the file with that name already exist");
  }

  auto parent_path = fs_.path_lookup(fs_.path_parent_dir(path));
  if (parent_path.empty()) {
    failure_message_ = "PATH NOT FOUND (neexistuje cilova cesta)";
    throw command_error("cannot reach parent of copied file inside filesystem");
  }

  auto parent_inode = parent_path.back();
  if (!fs_.dir_is(parent_inode)) {
    failure_message_ = "PATH NOT FOUND (neexistuje cilova cesta)";
    throw command_error("trying to use file as parent directory");
  }

  auto file_inode = fs_.file_create_sized(parent_inode, fs_.path_filename(path),
                                          input.size());
  fs_.file_write(file_inode, 0, reinterpret_cast<const char *>(input.data()),
                 input.size());
}

} // namespace jkfs
