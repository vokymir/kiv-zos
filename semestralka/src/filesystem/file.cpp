#include "filesystem.hpp"

namespace jkfs {

// TODO:
void Filesystem::file_create(int32_t parent_inode_id, std::string file_name) {}

// TODO:
void Filesystem::file_resize(int32_t inode_id, int32_t new_size) {}

// TODO:
void Filesystem::file_write(int32_t inode_id, int32_t offset, const char *data,
                            int32_t data_size) {}

// TODO:
std::vector<uint8_t> Filesystem::file_read(int32_t inode_id) { return {}; }

// TODO:
void Filesystem::file_delete(int32_t parent_inode_id, std::string file_name) {}

} // namespace jkfs
