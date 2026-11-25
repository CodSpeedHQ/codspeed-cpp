// Implementation
#include <cstdlib>
#include <filesystem>

#include "codspeed.h"
#include "utils.h"

namespace codspeed {

std::string safe_getenv(const char* var_name) {
#ifdef _WIN32
  // On Windows, use _dupenv_s instead of std::getenv for thread safety
  char* value;
  size_t len;
  errno_t err = _dupenv_s(&value, &len, var_name);
  if (err == 0 && value) {
    std::string result(value);
    free(value);
    return result;
  }
  return "";
#else
  const char* value = std::getenv(var_name);
  return value ? value : "";
#endif
}

std::string get_path_relative_to_workspace(const std::string& path,
                                           const std::string& compile_dir) {
  // 1. Check for bazel usage, through the BUILD_WORKSPACE_DIRECTORY env var
  // If so, __FILE__ will already be relative to the bazel workspace root
  if (!safe_getenv("BUILD_WORKSPACE_DIRECTORY").empty()) {
    return path;
  }

  // Tools like ccache with `base_dir` make __FILE__ relative to the
  // directory the compiler ran in, not to the one the benchmark runs in.
  // Other tools, like `-fmacro-prefix-map`, make it relative to another
  // directory, so this only uses `compile_dir` when the file exists there.
  std::filesystem::path file_path(path);
  if (file_path.is_relative() && !compile_dir.empty()) {
    auto in_compile_dir = std::filesystem::path(compile_dir) / file_path;
    std::error_code ec;
    if (std::filesystem::exists(in_compile_dir.lexically_normal(), ec)) {
      file_path = in_compile_dir;
    }
  }

  // 2. If defined, use the specificed value directly
#ifdef CODSPEED_ROOT_DIR
  return std::filesystem::relative(file_path, CODSPEED_ROOT_DIR).string();
#endif

  // 3. Fallback to bath relative to PWD
  return std::filesystem::relative(file_path, std::filesystem::current_path())
      .string();
}

}  // namespace codspeed
