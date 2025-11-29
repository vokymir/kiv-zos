#include "errors.hpp"
#include "filesystem.hpp"
#include <cstdint>
#include <string>

namespace jkfs {

void Filesystem::dir_create(int32_t parent_inode, std::string name) {
  if (parent_inode < 0) {
    throw jkfilesystem_error("Inode id must be >= 0, but is" +
                             std::to_string(parent_inode));
  }
}
} // namespace jkfs
