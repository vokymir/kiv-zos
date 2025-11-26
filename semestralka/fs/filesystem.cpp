#include "filesystem.hpp"
#include <filesystem>
#include <fstream>
#include <ios>
#include <mutex>
#include <string>

namespace jkfs {

// ===== singleton variables =====
Filesystem *Filesystem::instance_{nullptr};
std::mutex Filesystem::mutex_;

// ===== singleton behaviour =====
Filesystem::Filesystem(std::string &filename) {
  path_ = filename;
  file_ = std::fstream(path_, std::ios::binary | std::ios::in | std::ios::out);
}

Filesystem *Filesystem::instance(std::string &filename) {
  std::lock_guard<std::mutex> lock(mutex_);
  if (instance_ == nullptr) {
    instance_ = instance(filename);
  }
  return instance_;
}

// get/set
std::string Filesystem::path() const { return path_; }
void Filesystem::path(const std::string &path) {
  path_ = path;
  file_ = std::fstream(path_, std::ios::binary | std::ios::in | std::ios::out);
}

std::fstream &Filesystem::file() { return file_; }

} // namespace jkfs
