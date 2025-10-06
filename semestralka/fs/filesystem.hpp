#include <cstdint>

namespace jkfs{

// for each struct a class?

constexpr char ITEM_FREE = 0;
constexpr char ITEM_USED = 1;

struct superblock {
    char signature[16]; // author login
    std::int32_t disk_size; // celkova velikost VFS
    std::int32_t cluster_size;
    std::int32_t cluster_count;
    std::int32_t bitmapi_start_addr;
    std::int32_t bitmapd_start_addr;
    std::int32_t inode_start_addr;
    std::int32_t data_start_addr;
};

struct inode {
    std::int32_t node_id;
    char is_dir;
    std::int8_t references;
    std::int32_t file_size;
    std::int32_t direct1;
    std::int32_t direct2;
    std::int32_t direct3;
    std::int32_t direct4;
    std::int32_t direct5;
    std::int32_t indirect1;
    std::int32_t indirect2;
};

struct dir_item {
    std::int32_t inode;
    char item_name[12];
};

}
