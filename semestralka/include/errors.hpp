#include <exception>
#include <string>
namespace jkfs {

class jkfilesystem_error : public std::exception {
private:
  std::string message_;

public:
  jkfilesystem_error(const std::string &message) : message_(message) {}

  const char *what() const noexcept override { return message_.c_str(); }
};

class command_error : public std::exception {
private:
  std::string message_;

public:
  command_error(const std::string &message) : message_(message) {}

  const char *what() const noexcept override { return message_.c_str(); }
};

} // namespace jkfs
