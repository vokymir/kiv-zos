#include <filesystem>
#include <fstream>
#include <optional>
#include <string>

namespace jkfs {

class Filesystem {
private:
  std::filesystem::path path_;
  std::optional<std::fstream> file_;

public:
  Filesystem();

  // get/set path
  std::string path();
  void path(std::string path);
};

} // namespace jkfs
