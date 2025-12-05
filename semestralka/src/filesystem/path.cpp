#include "errors.hpp"
#include "filesystem.hpp"
#include <cstddef>
#include <cstdint>
#include <sstream>
#include <string>
#include <vector>

namespace jkfs {

int32_t Filesystem::path_lookup(std::string path) {
  if (path.empty()) {
    throw jkfilesystem_error("Cannot lookup empty path.");
  }

  std::vector<std::string> parts = path_split(path);
  // ERROR OUPUT: store which part of path was already traversed
  std::vector<std::string> traversed;

  // begin
  int32_t begin_id;
  int32_t curr_id;
  if (parts[0] == "/") {
    curr_id = root_id();
    traversed.push_back("/");
    // remove the "/" from parts
    parts.erase(parts.begin());
  } else {
    curr_id = current_inode_;
    traversed.push_back(".");
  }
  begin_id = curr_id;

  for (const auto &name : parts) {
    bool starting = begin_id == curr_id;
    curr_id = dir_lookup(curr_id, name);
    if (curr_id < 0) { // lookup failed
      if (starting) {
        throw jkfilesystem_error("Couldn't even start looking for path: " +
                                 path);
      }
      throw jkfilesystem_error("Cannot find path, wanted: '" + path +
                               "' but only found: '" + path_join(traversed) +
                               "'.");
    }
    traversed.push_back(name);
  }

  return curr_id;
}

// PRIVATE

std::vector<std::string> Filesystem::path_split(std::string path) {
  std::vector<std::string> parts;
  size_t start = 0;
  size_t end;

  // starting with root '/'
  while ((end = path.find('/', start)) != std::string::npos) {
    if (start == 0 && end == 0) { // if we are on root
      parts.push_back("/");
    } else {
      parts.push_back(path.substr(start, end - start));
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
    return "<GIVEN EMPTY PATH>";
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

} // namespace jkfs
