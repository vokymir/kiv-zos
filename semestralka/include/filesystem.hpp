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

enum class Bit_Order {
  LSB_FIRST,
  MSB_FIRST,
};

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
  bool vocal_ = true;
  int32_t cluster_size_ = 16; // TODO: make it 4096 after debug
  // how big can fs file be
  // superblock + 1 inode + 2 clusters + 1 byte for bitmapi + 1 byte for bitmapd
  // 1st cluster is reserved
  size_t min_size_ = sizeof(struct superblock) + sizeof(struct inode) +
                     static_cast<size_t>(cluster_size_) * 2 + 2;
  size_t max_size_ = INT32_MAX; // limit of 32bit addressing
  // i-nodes to data ratio
  double id_ratio_ = 0.05;
  static constexpr Bit_Order BIT_ORDER = Bit_Order::LSB_FIRST;

  // member variables
private:
  // file in which FS is saved
  std::filesystem::path path_;
  std::fstream file_;

  int32_t current_inode_ = 0; // always start at root

  // get/set
public:
  bool vocal() const;
  void vocal(bool vocal);
  std::string path() const;
  void path(const std::string &path);
  // stream cannot be changed to another file, but inside file whatever
  std::fstream &file();

  struct superblock superblock();
  void superblock(const struct superblock &sb);

  // return root directory inode
  struct inode root_inode();
  // return root directory inode id
  int32_t root_id();

  // methods
public:
  // == format ==

  // create file if it doesn't exist
  void filesystem_ensure();
  // resize filesystem to new size, does not gurantee data coherance except for
  // superblock - its existence is guaranteed. size must fulfil: fs_min_size <=
  // size <= fs_max_size
  void filesystem_resize(size_t size);

  // == inodes ==

  // find & return inode by its ID
  // ID = index in 'array of inodes'
  struct inode inode_read(int32_t inode_id);
  // find ID of first empty inode place
  // mark as used
  // return -1 if none found
  int32_t inode_alloc();
  // check if any inode is empty
  bool inode_is_empty(int32_t inode_id);
  // write inode to the address defined by its ID & to bitmap
  // WARN: will shamelessly overwrite existing data
  void inode_write(int32_t inode_id, const struct inode &new_inode);
  // clear the inode from bitmap/data
  void inode_free(int32_t inode_id);

  // == clusters ==

  // get vector of all bytes in cluster
  // doesn't matter if cluster is used or not
  std::vector<uint8_t> cluster_read(int32_t cluster_index);
  // find idx of first empty cluster
  // set cluster in bitmap as used
  // return -1 if none found
  int32_t cluster_alloc();
  // check if cluster at index is empty
  bool cluster_is_empty(int32_t cluster_index);
  // write raw data to a cluster on index
  // if size > cluster_size, throw error
  // if data == nullptr || size <= 0 only zero out the space
  // size is in bytes
  // WARN: will shamelessly overwrite existing data
  void cluster_write(int32_t cluster_index, const char *data,
                     int32_t data_size);
  // set cluster bitmap as unused
  // MAY or may NOT clear the memory
  void cluster_free(int32_t cluster_index);

  // == file ==
  // allocate inode & cluster, insert dir_item into parent dir & increase its
  // size; return file inode id
  // IS ATOMIC
  int32_t file_create(int32_t parent_inode_id, std::string file_name);
  // compute how many clusters needed, allocate or free them, update inode
  // (in)directs, update filesize
  // WARN: only can enlarge/extend file
  // WARN: Is really not atomic - on fail the system is corrupted
  void file_resize(int32_t inode_id, int32_t new_size);
  // writes accross multiple clusters
  // offset is in bytes - offset 15 means, that 15th byte will be the first to
  // be written to; if file is too small, will be resized
  void file_write(int32_t inode_id, int32_t offset, const char *data,
                  size_t data_size);
  // handle read accross multiple clusters
  std::vector<uint8_t> file_read(int32_t inode_id);
  // remove file & remove from parent directory
  // works on both files/directories
  void file_delete(int32_t parent_inode_id, std::string file_name);

  // == dir ==
  // allocate inode & cluster, insert dir_item into parent dir & increase its
  // size, insert . and .. into self
  // first call with parent_inode == 0 will setup root, all subsequent are
  // creating top-level files/dirs
  // IS ATOMIC
  void dir_create(int32_t parent_directory_inode_id,
                  std::string directory_name);
  // append one dir_item into any directory
  // IS ATOMIC
  void dir_item_add(int32_t directory_inode_id, int32_t item_inode_id,
                    std::string item_name);
  // remove one dir_item from directory
  void dir_item_remove(int32_t directory_inode_id, std::string item_name);
  // get ID (or -1) of any item inside directory stored in inode
  int32_t dir_lookup(int32_t directory_inode_id, std::string lookup_name);
  // list all dir_items in one directory
  std::vector<dir_item> dir_list(int32_t directory_inode_id);

  // == path ==
  // find any path and return its inode or -1
  int32_t path_lookup(std::string path);

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

  // == byte-wise ==

  // write raw bytes (but as char * because of stream) into anywhere
  void write_bytes(const char *data, size_t count, std::streamoff offset,
                   std::ios_base::seekdir way = std::ios::beg);

  // read raw bytes from FS, useful for bitmaps
  std::vector<uint8_t> read_bytes(size_t count, std::streamoff offset,
                                  std::ios_base::seekdir way = std::ios::beg);

  // == bit-wise ==

  // get/set/clear bit at index in byte - usefull for bitmaps not to be confused
  // by LSB or MSB ordering - that is retrieved from filesystem::BIT_ORDER
  // they take logical bit index and work with it in physical
  bool bit_get(uint8_t byte, uint32_t bit_index);
  void bit_set(uint8_t &byte, uint32_t bit_index);
  void bit_clear(uint8_t &byte, uint32_t bit_index);

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
  // print info about superblock usage based on sb and position after counting
  // all really used bytes in fs
  void print_sb_usage_info(struct superblock &sb, int32_t position) const;
  // create superblock for filesystem of given size with the use of
  struct superblock sb_from_size(int32_t size) const;

  // == path ==

  // split path from root, directory by dir, ending by dir or file
  // if path begins with '/' (root), will save it also, otherwise split by '/'
  std::vector<std::string> path_split(std::string path);
  // join path from parts
  // if path begins with "/" (root), will work with it
  std::string path_join(std::vector<std::string> parts);

  // == file ==
  // list all clusters which are used in the order
  std::vector<int32_t> file_list_clusters(int32_t inode_id);
  // list all clusters indexes (in order) which are stored in given cluster
  std::vector<int32_t> file_list_clusters_indirect(int32_t cluster_idx);

  // in file_resize() allocate more clusters if neccessary
  // return vector of cluster indexes
  // IS ATOMIC
  std::vector<int32_t> file_resize_allocate_clusters(int32_t inode_id,
                                                     int32_t new_size);
  // write all cluster indexes stored in to_write_from_back into cluster with
  // cluster_idx - that cluster is indirect and stores cluster indexes
  // IS ATOMIC
  void file_resize_cluster_indirect1(int32_t cluster_idx,
                                     std::vector<int32_t> &to_write_from_back);

  // write all cluster indexes stored in to_write_from_back into clusters
  // pointed to from clusters which are pointed from cluster_idx :D
  // IS ATOMIC
  void file_resize_cluster_indirect2(int32_t cluster_idx,
                                     std::vector<int32_t> &to_write_from_back);

  // write first cluster in file_write() - different behaviour than others,
  // because write only after offset bytes
  // return written_bytes
  // IS ATOMIC (by default)
  size_t file_write_first_cluster(int32_t cluster_idx,
                                  int32_t offset_in_cluster,
                                  const std::vector<uint8_t> &to_write);

  // write next clusters in file_write()
  // no read, only write
  // written_bytes are updated
  // IS ATOMIC (by default)
  void file_write_next_clusters(int32_t cluster_idx,
                                const std::vector<uint8_t> &to_write,
                                size_t &written_bytes);
};

} // namespace jkfs
