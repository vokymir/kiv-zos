#include "errors.hpp"
#include "filesystem.hpp"
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace jkfs {

std::vector<int32_t> Filesystem::path_lookup(std::string path) {
  std::vector<int32_t> inode_path;
  if (path.empty()) {
    return cwd_;
  }

  std::vector<std::string> parts = path_split(path);
  // ERROR OUPUT: store which part of path was already traversed
  std::vector<std::string> traversed;

  // determine STARTUP point
  if (parts[0] == "/") { // absolute path
    inode_path.push_back(root_id());
  } else { // relative path
    inode_path.insert(inode_path.end(), cwd_.begin(), cwd_.end());
  }

  for (const auto &name : parts) {
    auto curr_id = dir_lookup(inode_path.back(), name);
    if (curr_id < 0) { // lookup failed
      if (vocal_) {
        std::cout << "Cannot find path, wanted: '" << path
                  << "' but only found: '" << path_join(traversed) << "'."
                  << std::endl;
      }
      return {};
    }
    inode_path.push_back(curr_id);
    traversed.push_back(name);
  }

  path_make_flat(inode_path);

  return inode_path;
}

void Filesystem::path_make_flat(std::vector<int32_t> &inode_path) {
  // inode_id -> index in flattened path
  std::unordered_map<int32_t, size_t> seen;
  std::vector<int32_t> flattened;

  for (int32_t inode : inode_path) {
    auto it = seen.find(inode);

    // duplicate found => remove everything between first occurrence and this
    if (it != seen.end()) {

      size_t keep_idx = it->second;

      // remove entries from seen that are now obsolette (from the region in
      // between occurrences which will be removed)
      for (size_t i = flattened.size(); i > keep_idx + 1; i--) {
        seen.erase(flattened[i - 1]); // its 0-based
      }

      // truncate to keep only up to first occurrence
      flattened.resize(keep_idx + 1);
      continue; // skip adding that duplicate
    }

    // a) 1st occurrence
    // b) just removed everything in between, so it does nothing
    seen[inode] = flattened.size();
    flattened.push_back(inode);
  }

  inode_path = std::move(flattened);
}

// PRIVATE

std::vector<std::string> Filesystem::path_split(std::string path) {
  std::vector<std::string> parts;
  size_t start = 0;
  size_t end;

  parts.push_back(".");

  // starting with root '/'
  while ((end = path.find('/', start)) != std::string::npos) {
    if (start == 0 && end == 0) { // if we are on root
      parts.pop_back();           // remove the relative "."
      parts.push_back("/");
    } else {
      // skip double-slashes "...//..." as linux also does it
      std::string segment = path.substr(start, end - start);
      if (!segment.empty()) {
        parts.push_back(segment);
      }
    }
    start = end + 1;
  }
  // last part
  if (start < path.size()) {
    parts.push_back(path.substr(start));
  }

  return parts;
}

std::string Filesystem::path_join(std::vector<std::string> parts) {
  std::ostringstream oss;

  if (parts.empty()) {
    return "<EMPTY PATH>";
  } else if (parts.size() == 1) {
    // this is the case if only "/" is given, then the for-loop never runs
    return parts[0];
  }

  // a) is "smth but not root"
  // b) is "/"
  if (parts[0] != "/") {
    oss << parts[0];
  }
  parts.erase(parts.begin());

  // before 1st loop:
  // a) smth
  // b) (nothing)
  for (const auto &part : parts) {
    oss << "/" << part;
  }
  // after 1st loop:
  // a) smth/part
  // b) /part

  return oss.str();
}

std::string Filesystem::path_parent_dir(const std::string &path) {
  auto parts = path_split(path);
  if (parts.empty()) {
    throw jkfilesystem_error("given path have no parent");
  }
  parts.pop_back();
  return path_join(parts);
}

std::string Filesystem::path_filename(const std::string &path) {
  auto parts = path_split(path);
  if (parts.empty()) {
    throw jkfilesystem_error("given path have no filename");
  }
  return parts.back();
}

} // namespace jkfs
