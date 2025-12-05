#include "errors.hpp"
#include "filesystem.hpp"
#include "structures.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <ranges>
#include <tuple>
#include <utility>
#include <vector>

namespace jkfs {

int32_t Filesystem::file_create(int32_t parent_id, std::string file_name) {
  int32_t inode = -1;
  int32_t cluster = -1;

  try {
    inode = inode_alloc();
    if (inode < 0) {
      throw jkfilesystem_error("There is no empty inode for file.");
    }
    cluster = cluster_alloc();
    if (cluster < 0) {
      throw jkfilesystem_error("There is no empty cluster for file.");
    }

    // fill inode
    struct inode file{};
    file.node_id = inode;
    file.is_dir = false;
    file.file_size = 0;
    file.direct[0] = cluster;

    // write inode
    inode_write(inode, file);

    // add this to parent dir
    dir_item_add(parent_id, inode, file_name);

    // return inode id
    return inode;
  } catch (...) {
    // rollback
    file_delete(parent_id, file_name);

    // let others know the exception
    throw;
  }
}

void Filesystem::file_ensure_size(int32_t inode_id, int32_t new_size) {
  // count how many clusters are needed
  auto need = file_ensure_size__count_clusters(new_size);
  if (!need.possible) {
    throw jkfilesystem_error("Not enough space.");
  }

  // prepare for new allocations
  auto have = file_list_clusters(inode_id);
  auto have_data = std::get<0>(have);
  auto have_overhead = std::get<1>(have);

  // is used in backup
  struct inode inode;
  std::vector<int32_t> new_data;
  std::vector<int32_t> new_overhead;
  int backup_stage = 0;

  try {
    // allocate
    std::tie(new_data, new_overhead) =
        file_ensure_size__fill(need, have_data.size(), have_overhead.size());
    backup_stage = 1;

    // write all

    // = prepare where to write
    inode = inode_read(inode_id);

    // = prepare what to write
    std::vector<int32_t> join_data =
        file_ensure_size__join(have_data, new_data);
    std::vector<int32_t> join_overhead =
        file_ensure_size__join(have_overhead, new_overhead);

    // = write only overhead clusters
    file_ensure_size__write_overhead_clusters(inode, join_overhead);
    backup_stage = 2;
    // = write only data clusters
    file_ensure_size__write_clusters_data(inode, join_data, join_overhead);
    backup_stage = 3;
    // = write inode back to fs
    inode.file_size = new_size;
    inode_write(inode.node_id, inode);
  } catch (...) {
    // go from back & reverse all the actions to be in the before state
    switch (backup_stage) {
    case 3:
      file_ensure_size__write_clusters_data(inode, have_data, have_overhead);
    case 2:
      file_ensure_size__write_overhead_clusters(inode, have_overhead);
    case 1:
      for (const auto &cluster : new_data) {
        cluster_free(cluster);
      }
      for (const auto &cluster : new_overhead) {
        cluster_free(cluster);
      }
    case 0:
    default:
      break;
    }

    throw;
  }
}

void Filesystem::file_write(int32_t inode_id, int32_t offset, const char *data,
                            size_t data_size) {
  // if dont have enough space, resize
  file_ensure_size(inode_id, static_cast<int32_t>(data_size) + offset);

  // list all clusters to write into
  auto clusters_tuple = file_list_clusters(inode_id);
  // only take data clusters
  auto clusters = std::get<0>(clusters_tuple);

  size_t start_cluster_idx = static_cast<size_t>(offset / cluster_size_);
  size_t start_cluster_offset = static_cast<size_t>(offset % cluster_size_);

  // look at the data differently
  std::span<uint8_t> data_to_write(
      reinterpret_cast<uint8_t *>(const_cast<char *>(data)), data_size);

  size_t written_bytes = 0;
  size_t cluster_idx = start_cluster_idx;

  while (written_bytes < data_size) {
    // get current cluster index
    auto cluster = clusters[cluster_idx];
    // if 1st cluster: give offset, otherwise 0
    auto cluster_offset =
        start_cluster_idx == cluster_idx ? start_cluster_offset : 0;

    file_write__cluster(cluster, cluster_offset, data_to_write, written_bytes);

    cluster_idx++;
  }
}

std::vector<uint8_t> Filesystem::file_read(int32_t inode_id) {
  auto file_size = inode_read(inode_id).file_size;

  auto clusters = file_list_clusters(inode_id);
  auto data_clusters = std::get<0>(clusters);

  std::vector<uint8_t> output;
  output.reserve(file_size);

  size_t remaining = file_size;

  for (const auto &cluster : data_clusters) {
    if (remaining == 0) {
      break;
    }

    auto data = cluster_read(cluster);

    size_t take = std::min(remaining, data.size());
    output.insert(output.end(), data.begin(), data.begin() + take);

    remaining -= take;
  }

  return output;
}

void Filesystem::file_delete(int32_t parent_inode_id, std::string file_name) {
  // find inode to delete
  auto inode = dir_lookup(parent_inode_id, file_name);
  if (inode < 0) {
    return; // already done
  }

  // free clusters
  auto clusters = file_list_clusters(inode);
  auto data_clusters = std::get<0>(clusters);
  for (const auto &cluster : data_clusters) {
    cluster_free(cluster);
  }

  // free inode
  inode_free(inode);
  // free dir item reference
  dir_item_remove(parent_inode_id, file_name);
}

// PRIVATE

std::tuple<std::vector<int32_t>, std::vector<int32_t>>
Filesystem::file_list_clusters(int32_t inode_id) {
  std::vector<int32_t> data;
  std::vector<int32_t> overhead;
  auto inode = inode_read(inode_id);

  // direct
  for (const auto &idx : inode.direct) {
    if (idx <= 0) {
      return {data, overhead}; // already have all
    }
    data.push_back(idx);
  }

  // indirect 1
  if (inode.indirect1 <= 0) {
    return {data, overhead}; // already have all
  }
  overhead.push_back(inode.indirect1); // STORE OVERHEAD CLUSTERS
  auto indirect1 = file_list_clusters__indirect(inode.indirect1);
  data.insert(data.end(), indirect1.begin(), indirect1.end());

  // indirect 2
  if (inode.indirect2 <= 0) {
    return {data, overhead}; // already have all
  }
  overhead.push_back(inode.indirect2); // STORE OVERHEAD CLUSTERS
  // load where to look
  auto indirects = file_list_clusters__indirect(inode.indirect2);
  // STORE OVERHEAD CLUSTERS
  overhead.insert(overhead.end(), indirects.begin(), indirects.end());
  // look there
  for (const auto &indirect : indirects) {
    auto indirect2 = file_list_clusters__indirect(indirect);
    if (indirect2.empty()) {
      return {data, overhead}; // already have all
    }
    data.insert(data.end(), indirect2.begin(), indirect2.end());
  }

  return {data, overhead};
}

std::vector<int32_t>
Filesystem::file_list_clusters__indirect(int32_t cluster_idx) {
  std::vector<int32_t> clusters;

  auto bytes = cluster_read(cluster_idx);
  std::span<const int32_t> cluster_idxs{
      reinterpret_cast<const int32_t *>(bytes.data()),
      bytes.size() / sizeof(int32_t)};

  for (const auto &idx : cluster_idxs) {
    if (idx <= 0) {
      return clusters;
    }
    clusters.push_back(idx);
  }

  return clusters;
}

void Filesystem::file_write__cluster(int32_t cluster_idx,
                                     int32_t offset_in_cluster,
                                     const std::span<uint8_t> &data_to_write,
                                     size_t &written_bytes) {
  // always read - because this function should not discard other data
  std::vector<uint8_t> raw = cluster_read(cluster_idx);

  // start somewhere, end correctly
  for (int i = offset_in_cluster; i < cluster_size_; i++) {
    if (written_bytes >= data_to_write.size()) {
      break; // already wrote everything
    }

    raw[static_cast<size_t>(i)] = data_to_write[written_bytes];
    written_bytes++;
  }

  // write to cluster
  cluster_write(cluster_idx, reinterpret_cast<char *>(raw.data()),
                cluster_size_);
}

struct Needed_Clusters
Filesystem::file_ensure_size__count_clusters(int32_t size) {
  Needed_Clusters result{};

  // ceil of how many data clusters needed
  size_t data = (size + cluster_size_ - 1) / cluster_size_;
  result.data = data;

  // direct
  size_t direct = std::min(std::size(inode{}.direct), data);
  result.direct = direct;
  data -= direct;

  if (data < 0) {
    throw jkfilesystem_error("I cannot count to ten.");
  }
  if (data == 0) {
    return result; // fits inside direct clusters
  }

  // indirect 1
  size_t indirect1 = 1; // if we are still here, it is needed
  // floor of how many dir_items fit inside one cluster
  const size_t data_in_cluster = cluster_size_ / sizeof(dir_item);
  result.indirect1 = indirect1;
  data -= data_in_cluster;

  // in indirect1 can be some space left, therefore also <
  if (data <= 0) {
    return result;
  }

  // indirect 2
  size_t indirect2 = 1; // the same logic - we are here, so its needed
  // floor of how many indirect1 pointers fit inside one indirect2
  const size_t clusters_in_cluster = cluster_size_ / sizeof(int32_t);
  // ceil of how many indirect1s are needed inside indirect2
  size_t in1_inside_in2 = (data + data_in_cluster - 1) / data_in_cluster;

  // the number of indirect1s is too big to fit inside indirect2
  if (in1_inside_in2 > clusters_in_cluster) {
    result.possible = false;
    return result;
  }

  result.indirect1 += in1_inside_in2;
  result.indirect2 = indirect2;
  data -= data_in_cluster * in1_inside_in2;

  return result;
}

std::tuple<std::vector<int32_t>, std::vector<int32_t>>
Filesystem::file_ensure_size__fill(const struct Needed_Clusters &need,
                                   const size_t have_data_size,
                                   const size_t have_overhead_size) {
  auto need_overhead = need.indirect1 + need.indirect2;

  std::vector<int32_t> new_data;
  std::vector<int32_t> new_overhead;

  new_data.reserve(need.data - have_data_size);
  new_overhead.reserve(need_overhead - have_overhead_size);

  try {
    // data
    for (auto i = have_data_size; i < need.data; i++) {
      auto cluster = cluster_alloc();
      if (cluster == -1) {
        throw jkfilesystem_error("Cannot allocate more clusters for data...");
      }
      new_data.push_back(cluster);
    }

    // overhead
    for (auto i = have_overhead_size; i < need_overhead; i++) {
      auto cluster = cluster_alloc();
      if (cluster == -1) {
        throw jkfilesystem_error("Cannot allocate more clusters for overhead.");
      }
      new_overhead.push_back(cluster);
    }

  } catch (...) {
    // free all newly allocated clusters
    for (const auto &cluster : new_data) {
      cluster_free(cluster);
    }
    for (const auto &cluster : new_overhead) {
      cluster_free(cluster);
    }

    throw;
  }

  return {new_data, new_overhead};
}

std::vector<int32_t>
Filesystem::file_ensure_size__join(std::vector<int32_t> &first,
                                   std::vector<int32_t> &second) {
  std::vector<int32_t> result;
  result.reserve(first.size() + second.size());

  result.insert(result.end(), first.begin(), first.end());
  result.insert(result.end(), second.begin(), second.end());

  return result;
}

void Filesystem::file_ensure_size__write_overhead_clusters(
    struct inode &inode, std::vector<int32_t> &clusters) {
  auto idx = 0;
  struct inode backup_inode;
  backup_inode.copy_from(inode);
  std::vector<uint8_t> backup_indirect2;
  bool failed_in_indirect2 = false;

  try {
    // indirect 1
    if (!(clusters.size() > idx)) {
      return;
    }
    inode.indirect1 = clusters[idx++];

    // indirect 2
    if (!(clusters.size() > idx)) {
      return;
    }
    inode.indirect2 = clusters[idx++];

    // can fit inside ind2 ?
    auto overhead_left = clusters.size() - idx;
    auto max_ind1_in_ind2 = cluster_size_ / sizeof(clusters[0]);
    if (overhead_left > max_ind1_in_ind2) {
      throw jkfilesystem_error(
          "Cannot fit indirect1s inside indirect2 - file too big");
    }

    // write all ind1 to ind2
    backup_indirect2 = cluster_read(inode.indirect2);
    failed_in_indirect2 = true;
    cluster_write(inode.indirect2,
                  reinterpret_cast<const char *>(clusters[idx]),
                  overhead_left * sizeof(clusters[0]));
  } catch (...) {
    if (failed_in_indirect2) {
      // restore indirect2 cluster
      cluster_write(inode.indirect2,
                    reinterpret_cast<const char *>(backup_indirect2.data()),
                    backup_indirect2.size() * sizeof(backup_indirect2[0]));
    }

    // restore inode
    inode.copy_from(backup_inode);

    throw;
  }
}

void Filesystem::file_ensure_size__write_clusters_data(
    struct inode &inode, std::vector<int32_t> &data,
    const std::vector<int32_t> &overhead) {
  auto idx = 0;
  auto overhead_idx = 0;
  struct inode backup_inode;
  backup_inode.copy_from(inode);
  // vector of [cluster idx, raw indirect1 file]
  std::vector<std::pair<size_t, std::vector<uint8_t>>> backup_indirect_1s;

  try {
    // direct
    for (auto i = 0; i < std::size(inode.direct); i++) {
      if (idx >= data.size()) {
        return; // work already done
      }
      inode.direct[i] = data[idx++];
    }

    // indirect 1
    auto max_data_in_ind1 = cluster_size_ / sizeof(data[0]);
    auto amount_to_ind1 = std::min(max_data_in_ind1, data.size() - idx);

    backup_indirect_1s.push_back(
        {overhead[overhead_idx], cluster_read(overhead[overhead_idx])});
    cluster_write(overhead[overhead_idx++],
                  reinterpret_cast<const char *>(data[idx]),
                  amount_to_ind1 * sizeof(data[idx]));
    idx += amount_to_ind1;

    // indirect 2
    overhead_idx++; // skip indirect2
    for (auto i = overhead_idx; i < overhead.size(); i++) {
      auto ind1 = overhead[overhead_idx];

      if (idx >= data.size()) {
        return; // work already done
      }

      // overwrite indirect 1
      amount_to_ind1 = std::min(max_data_in_ind1, data.size() - idx);
      backup_indirect_1s.push_back({ind1, cluster_read(ind1)});
      cluster_write(overhead[overhead_idx++],
                    reinterpret_cast<const char *>(data[idx]),
                    amount_to_ind1 * sizeof(data[idx]));

      // go to next
      overhead_idx++;
      idx += amount_to_ind1;
    }

  } catch (...) {
    // restore inode
    inode.copy_from(backup_inode);

    // restore all overwritten indirect 1 clusters
    for (const auto &[idx, raw] : backup_indirect_1s) {
      cluster_write(idx, reinterpret_cast<const char *>(raw.data()),
                    raw.size() * sizeof(raw[0]));
    }

    throw;
  }
}

} // namespace jkfs
