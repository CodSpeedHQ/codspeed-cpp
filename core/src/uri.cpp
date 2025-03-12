#include "codspeed.h"
#include <string>
#include <iostream>

std::string extract_lambda_namespace(const std::string& pretty_func) {
  if (pretty_func.find("(anonymous namespace)") != std::string::npos) {
    std::cerr << "[ERROR] Anonymous namespace not supported in " << pretty_func << std::endl;
    return {};
  }

#ifdef __clang__
  // Example: auto outer::test12::(anonymous class)::operator()() const

  std::size_t anon_class_pos = pretty_func.find("(anonymous class)");
  std::size_t space_pos = pretty_func.find(' ');

  if (space_pos == std::string::npos || anon_class_pos == std::string::npos) {
    return {};
  }

  return pretty_func.substr(space_pos, anon_class_pos - space_pos) + "::";
#elif __GNUC__
    // Example: outer::test12::<lambda()>

    auto lambda_pos = pretty_func.find("::<lambda()>");
    if (lambda_pos == std::string::npos) {
        return {};
    }

    return pretty_func.substr(0, lambda_pos) + "::";
#else
#error "Unsupported compiler"
#endif
}
