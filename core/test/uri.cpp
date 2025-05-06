#include <gtest/gtest.h>

#include "codspeed.h"

// Manual definition (to avoid including it in the public header):
namespace codspeed {
std::string extract_namespace_clang(const std::string &func_str);
std::string extract_namespace_gcc(const std::string &func_str);
}  // namespace codspeed

TEST(UriTest, TestExtractNamespaceClang) {
  EXPECT_EQ(codspeed::extract_namespace_clang(
                "auto outer::test12::(anonymous class)::operator()() const"),
            "outer::test12::");
  EXPECT_EQ(codspeed::extract_namespace_clang(
                "auto outer::(anonymous namespace)::test12::(anonymous "
                "class)::operator()() const"),
            "outer::(anonymous namespace)::test12::");
}

TEST(UriTest, TestExtractNamespaceGcc) {
  EXPECT_EQ(codspeed::extract_namespace_gcc("outer::test12::<lambda()>"),
            "outer::test12::");
  EXPECT_EQ(codspeed::extract_namespace_gcc(
                "outer::(anonymous namespace)::test12::<lambda()>"),
            "outer::(anonymous namespace)::test12::");
}

namespace a {
namespace b {
namespace c {
static std::string pretty_func = ([]() { return __PRETTY_FUNCTION__; })();

TEST(UriTest, TestExtractNamespace) {
  EXPECT_EQ(codspeed::extract_lambda_namespace(pretty_func), "a::b::c::");
}
}  // namespace c
}  // namespace b
}  // namespace a
