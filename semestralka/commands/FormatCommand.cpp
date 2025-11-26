#include <cctype>
#include <cstddef>
#include <stdexcept>
#include <string>
#include <tuple>

#include "commands.hpp"

namespace jkfs {

// ===== LOCAL =====

// File size that the file can be formatted to.
enum class FILE_SIZE {
  B,
  KB,
  MB,
  GB,
};

// convert from string to enum, optionally can start parsing string on position
// greater than 0
FILE_SIZE convert_filesize(std::string &str, size_t pos = 0) {
  char ch;
  ch = static_cast<char>(tolower(str[pos]));
  switch (ch) {
  case 'b':
    return FILE_SIZE::B;
  case 'k':
    return FILE_SIZE::KB;
  case 'm':
    return FILE_SIZE::MB;
  case 'g':
    return FILE_SIZE::GB;
  default:
    throw std::logic_error("The size is not one of known (e.g. KB, MB,...)");
  }
}

// from string, e.g. 600MB convert to 600 and MB
std::tuple<int, FILE_SIZE> get_size(std::string &arg) {
  size_t pos = 0;
  int number = std::stoi(arg, &pos);
  FILE_SIZE size = convert_filesize(arg, pos);

  return {number, size};
}

// ===== GLOBAL =====

FormatCommand::FormatCommand() {
  id_ = "format";
  name_ = "Format filesystem";
  desc_ = "Format the file given as a parameter at the start of the program.\n\
| Format to the filesystem of given size. \n\
| If file already existed (no matter if was filesystem or not) it will be overwritten,\n\
| if didn't exist it will be created.";
  how_ = "format 600MB // format the file to be filesystem of 600 Mebibyte";
}

void FormatCommand::execute(std::vector<std::string> &args) noexcept {
  int number;
  FILE_SIZE size;
  std::tie(number, size) = get_size(args[0]);
}

} // namespace jkfs
