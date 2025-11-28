#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <ios>
#include <mutex>
#include <stdexcept>
#include <string>

#include "structures.hpp"

namespace jkfs {

// CONCEPTS are used for read/write into filesystem file

// don't have custom copy semantics, internal pointers, dynamic memory, ... &&
// objects memory is predictable
template <typename T>
concept Trivially_Serializable =
    std::is_trivially_copyable_v<T> && std::is_standard_layout_v<T>;

// is not a pointer nor a reference
template <typename T>
concept Not_Pointer_Or_Reference =
    !std::is_pointer_v<T> && !std::is_reference_v<T>;

// can be serialized trivially and is not a pointer or reference
template <typename T>
concept Raw_Writable = Trivially_Serializable<T> && Not_Pointer_Or_Reference<T>;

// class representing the filesystem exposing API which is used by commands
class Filesystem {
  // singleton behaviour
private:
  static Filesystem *instance_;
  static std::mutex mutex_;
  Filesystem(const std::string &filename);
  ~Filesystem() = default;

public:
  // initialize singleton
  static Filesystem &instance(const std::string &filename);
  // get instance
  static Filesystem &instance();
  Filesystem(Filesystem &other) = delete;
  void operator=(const Filesystem &other) = delete;

  // config variables
private:
  int32_t cluster_size_ = 4096;
  // how big can fs file be
  size_t min_size_ = sizeof(struct superblock) + sizeof(struct inode) +
                     static_cast<size_t>(cluster_size_) + 2;
  size_t max_size_ = INT32_MAX; // limit of 32bit addressing
  // i-nodes to data ratio
  double id_ratio_ = 0.2;

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

  struct superblock superblock();
  void superblock(const struct superblock &sb);

  // methods
public:
  // == FORMAT ==

  void ensure_file();
  // resize filesystem to new size, does not gurantee data coherance except for
  // superblock - its existence is guaranteed. size must fulfil: fs_min_size <=
  // size <= fs_max_size
  void resize_file(size_t size);

  // private methods
private:
  // write anything into file - beware: if structure is something more complex,
  // make sure it can be casted into <const char *>
  template <Raw_Writable T>
  void write(const T &structure, std::streamoff offset,
             std::ios_base::seekdir way) {
    file_.clear();

    file_.seekp(offset, way);
    file_.write(reinterpret_cast<const char *>(&structure), sizeof(T));

    if (!file_) {
      throw std::runtime_error("Cannot write into file.");
    }

    file_.flush();
  }

  // read anything from file - beware: structure T *MUST* be constructable via
  // 'T name{};' and be castable to <char *>
  template <Raw_Writable T>
  T read(std::streamoff offset, std::ios_base::seekdir way) {
    T structure{};
    file_.clear();

    file_.seekg(offset, way);
    file_.read(reinterpret_cast<char *>(&structure), sizeof(T));

    if (!file_) {
      throw std::runtime_error("Cannot read from file.");
    }

    return structure;
  }

  // == FORMAT ==

  // return max count of items if each item must have record in bitmap
  int32_t iterative_count_max(int32_t available_space, int32_t item_size) const;
  // max count of clusters - uses id_ratio_
  int32_t count_clusters(int32_t available_space) const;
  // max count of inodes - uses cluster_size_
  int32_t count_inodes(int32_t available_space, int32_t cluster_count) const;
  // print info about superblock & usage based on sb and position after counting
  // all really used bytes in fs
  void print_sb_usage_info(struct superblock &sb, int32_t position) const;
  // create superblock for filesystem of given size with the use of
  struct superblock sb_from_size(int32_t size) const;
};

} // namespace jkfs
