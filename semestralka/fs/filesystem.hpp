#include <filesystem>
#include <fstream>
#include <mutex>
#include <string>

namespace jkfs {

class Filesystem {
  // singleton behaviour
private:
  static Filesystem *instance_;
  static std::mutex mutex_;
  Filesystem(std::string &filename);
  ~Filesystem() = default;

public:
  static Filesystem *instance(std::string &filename);
  Filesystem(Filesystem &other) = delete;
  void operator=(const Filesystem &other) = delete;

  // member variables
private:
  std::filesystem::path path_;
  std::fstream file_;

  // get/set
public:
  std::string path() const;
  void path(const std::string &path);
  // stream cannot be changed to another file, but inside file whatever
  std::fstream &file();

  // methods
public:
};

} // namespace jkfs
