#include <cctype>
#include <cstddef>
#include <cstring>
#include <string>
#include <tuple>

#include "commands.hpp"
#include "errors.hpp"

namespace jkfs {

FormatCommand::FormatCommand() {
  id_ = "format";
  name_ = "Format filesystem";
  desc_ = "Format the file given as a parameter at the start of the program.\n\
| Format to the filesystem of given size. \n\
| If file already existed (no matter if was filesystem or not) it will be overwritten,\n\
| if didn't exist it will be created.";
  how_ = "format 600MB // format the file to be filesystem of 600 Mebibyte";

  success_message_ = "OK";
  failure_message_ = "CANNOT CREATE FILE";
}

void FormatCommand::execute_inner(const std::vector<std::string> &args) {
  if (args.empty()) {
    throw command_error("The FORMAT command require one argument. See "
                        "help (format -h) for more info.");
  }

  int size = get_total_size(args[0]);
  fs_.filesystem_resize(static_cast<size_t>(size));
}

// PRIVATE

int FormatCommand::get_total_size(const std::string &arg) {
  auto [number, size] = get_size(arg);

  return number * convert_size(size);
}

std::tuple<int, FILE_SIZE> FormatCommand::get_size(const std::string &arg) {
  size_t pos = 0;
  int number = std::stoi(arg, &pos);
  FILE_SIZE size = convert_filesize(arg, pos);

  return {number, size};
}

FILE_SIZE FormatCommand::convert_filesize(const std::string &str, size_t pos) {
  char ch;
  ch = static_cast<char>(tolower(str[pos]));
  switch (ch) {
  case 'b':
    return FILE_SIZE::B;
  case 'k':
    return FILE_SIZE::KB;
  case 'm':
    return FILE_SIZE::MB;
  default:
    throw command_error("The size is not one of known (e.g. KB, MB, ...)");
  }
}

int FormatCommand::convert_size(enum FILE_SIZE fs) {
  switch (fs) {
  case FILE_SIZE::B:
    return 1;
  case FILE_SIZE::KB:
    return 1'000;
  case FILE_SIZE::MB:
    return 1'000'000;
  default:
    throw command_error("The size if not one of known (e.g. KB, MB, ...)");
  }
}

} // namespace jkfs
