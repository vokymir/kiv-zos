#include "filesystem.hpp"
#include "structures.hpp"
#include <filesystem>
#include <fstream>
#include <ios>
#include <iostream>
#include <mutex>
#include <stdexcept>
#include <string>

namespace jkfs {

// ===== singleton variables =====
Filesystem *Filesystem::instance_{nullptr};
std::mutex Filesystem::mutex_;

// ===== singleton behaviour =====
Filesystem::Filesystem(const std::string &filename) { path(filename); }

Filesystem &Filesystem::instance(const std::string &filename) {
  std::lock_guard<std::mutex> lock(mutex_);
  if (instance_ == nullptr) {
    instance_ = new Filesystem(filename);
  }
  return *instance_;
}

Filesystem &Filesystem::instance() {
  std::lock_guard<std::mutex> lock(mutex_);
  if (instance_ == nullptr) {
    throw std::runtime_error(
        "Cannot get singleton instance if it wasn't initialized.");
  }
  return *instance_;
}

// get/set
std::string Filesystem::path() const { return path_; }
void Filesystem::path(const std::string &path) {
  path_ = path;
  file_.close();
  file_ = std::fstream(path_, std::ios::binary | std::ios::in | std::ios::out);
}

std::fstream &Filesystem::file() { return file_; }

struct superblock Filesystem::superblock() {
  return read<struct superblock>(0, std::ios::beg);
}

void Filesystem::superblock(const struct superblock &sb) {
  write<struct superblock>(sb, 0, std::ios::beg);
}

// ===== methods =====

void Filesystem::ensure_file() {
  if (!std::filesystem::exists(path_)) {
    file_.close();
    std::ofstream o(path_);
    o.close();
    path(path_);
  }
}

void Filesystem::resize_file(size_t size) {

  if (fs_min_size > size || size > fs_max_size) {
    throw std::runtime_error("The size is too big/small.");
  }

  file_.close();
  std::filesystem::resize_file(path_, static_cast<uintmax_t>(size));
  path(path_);
}

} // namespace jkfs
