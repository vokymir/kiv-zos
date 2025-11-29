#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <ios>
#include <mutex>
#include <stdexcept>
#include <string>
#include <vector>

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

  // == inodes ==

  // find & return inode by its ID
  // ID = index in 'array of inodes'
  struct inode inode_get(int32_t id);
  // find ID of first empty inode place
  // return -1 if none found
  int32_t inode_get_empty();
  // check if any inode is empty
  bool inode_is_empty(int32_t id);
  // write inode to the address defined by its ID & to bitmap
  // WARN: will shamelessly overwrite existing data
  void inode_write(int32_t id, const struct inode &i);
  // clear the inode from bitmap/data
  void inode_free(int32_t id);

  // == clusters ==

  // get vector of all bytes in cluster
  // doesn't matter if cluster is used or not
  std::vector<uint8_t> cluster_get(int32_t idx);
  // find idx of first empty cluster
  // return -1 if none found
  int32_t cluster_get_empty();
  // check if cluster at index is empty
  bool cluster_is_empty(int32_t idx);
  // write raw data to a cluster on index
  // if size > cluster_size, throw error
  // size is in bytes
  // WARN: will shamelessly overwrite existing data
  void cluster_write(int32_t idx, const char *data, int32_t size);
  // set cluster bitmap as unused
  // MAY or may NOT clear the memory
  void cluster_free(int32_t idx);

  // private methods
private:
  // write anything into file - beware: if structure is something more
  // complex, make sure it can be casted into <const char *>
  template <Raw_Writable STRUCTURE>
  void write(const STRUCTURE &structure, std::streamoff offset,
             std::ios_base::seekdir way = std::ios::beg) {
    file_.clear();

    file_.seekp(offset, way);
    file_.write(reinterpret_cast<const char *>(&structure), sizeof(structure));

    if (!file_) {
      throw std::runtime_error("Cannot write into file.");
    }

    file_.flush();
  }

  // read anything from file - beware: structure T *MUST* be constructable via
  // 'T name{};' and be castable to <char *>
  template <Raw_Writable STRUCTURE>
  STRUCTURE read(std::streamoff offset,
                 std::ios_base::seekdir way = std::ios::beg) {
    STRUCTURE s{};
    file_.clear();

    file_.seekg(offset, way);
    file_.read(reinterpret_cast<char *>(&s), sizeof(s));

    if (!file_) {
      throw std::runtime_error("Cannot read from file.");
    }

    return s;
  }

  // write raw bytes (but as char * because of stream) into anywhere
  void write_bytes(const char *data, size_t count, std::streamoff offset,
                   std::ios_base::seekdir way = std::ios::beg);

  // read raw bytes from FS, useful for bitmaps
  std::vector<uint8_t> read_bytes(size_t count, std::streamoff offset,
                                  std::ios_base::seekdir way = std::ios::beg);

  // get first bit in vector which is 0 or 1 based on value
  // return index of bit, classic 0-indexed
  // return -1 if value not found in vec
  int32_t get_first_bit(std::vector<uint8_t> &vec, bool value);

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
