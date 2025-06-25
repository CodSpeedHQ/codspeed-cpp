#include <iostream>
#include <string>

#include "codspeed.h"

namespace codspeed {

// Example: auto outer::test12::(anonymous class)::operator()() const
// Returns: outer::test12::
std::string extract_namespace_clang(const std::string &pretty_func) {
  std::size_t anon_class_pos = pretty_func.find("::(anonymous class)");
  std::size_t space_pos = pretty_func.find(' ');

  if (space_pos == std::string::npos || anon_class_pos == std::string::npos) {
    return {};
  }
  space_pos += 1;  // Skip the space

  return pretty_func.substr(space_pos, anon_class_pos - space_pos) + "::";
}

// Example: outer::test12::<lambda()>
// Returns: outer::test12::
std::string extract_namespace_gcc(const std::string &pretty_func) {
  auto lambda_pos = pretty_func.find("::<lambda()>");
  if (lambda_pos == std::string::npos) {
    return {};
  }

  return pretty_func.substr(0, lambda_pos) + "::";
}

// Has to pass the pretty function from a lambda:
//   (([]() { return __PRETTY_FUNCTION__; })())
//
// Returns: An empty string if the namespace could not be extracted,
//         otherwise the namespace with a trailing "::"
std::string extract_lambda_namespace(const std::string &pretty_func) {
  if (pretty_func.find("(anonymous namespace)") != std::string::npos) {
    std::cerr << "[ERROR] Anonymous namespace not supported in " << pretty_func
              << std::endl;
    return {};
  }

#ifdef __clang__
  return extract_namespace_clang(pretty_func);
#elif __GNUC__
  return extract_namespace_gcc(pretty_func);
#elif _MSC_VER
  // MSVC doesn't support __PRETTY_FUNCTION__ in the same way
  // Return empty string as fallback for Windows
  return {};
#else
#error "Unsupported compiler"
#endif
}

}  // namespace codspeed
