// Implementation
#include <cstdlib>
#include <filesystem>

#include "codspeed.h"

namespace codspeed {

std::string get_path_relative_to_workspace(const std::string &path) {
  // 1. Check for bazel usage, through the BUILD_WORKSPACE_DIRECTORY env var
  // If so, __FILE__ will already be relative to the bazel workspace root
  if (std::getenv("BUILD_WORKSPACE_DIRECTORY") != NULL) {
    return path;
  }

  // 2. If defined, use the specificed value directly
#ifdef CODSPEED_ROOT_DIR
  return std::filesystem::relative(path, CODSPEED_ROOT_DIR).string();
#endif

  // 3. Fallback to bath relative to PWD
  return std::filesystem::relative(path, std::filesystem::current_path())
      .string();
}

}  // namespace codspeed
