#include "codspeed.h"
#include <gtest/gtest.h>

// Manual definition (to avoid including it in the public header):

TEST(CodSpeedTest, TestSearchAndReplaceBetweenBracketsNamespace) {
  std::string no_brackets_input =
      "examples/google_benchmark/main.cpp::BM_rand_vector";
  std::string no_brackets_output =
      "examples/google_benchmark/main.cpp::BM_rand_vector";
  EXPECT_EQ(sanitize_bench_args(no_brackets_input), no_brackets_output);

  std::string brackets_and_no_escaped_type_input =
      "examples/google_benchmark/"
      "template_bench.hpp::BM_Template1_Capture[two_type_test, int, double]";
  std::string brackets_and_no_escaped_type_output =
      "examples/google_benchmark/"
      "template_bench.hpp::BM_Template1_Capture[two_type_test, int, double]";
  EXPECT_EQ(sanitize_bench_args(brackets_and_no_escaped_type_input),
            brackets_and_no_escaped_type_output);

  std::string brackets_and_escaped_type_input =
      "examples/google_benchmark/"
      "template_bench.hpp::test::BM_Template[std::string]";
  std::string brackets_and_escaped_type_output =
      "examples/google_benchmark/"
      "template_bench.hpp::test::BM_Template[std\\:\\:string]";

  EXPECT_EQ(sanitize_bench_args(brackets_and_escaped_type_input),
            brackets_and_escaped_type_output);

  std::string brackets_and_escaped_types_input =
      "examples/google_benchmark/"
      "template_bench.hpp::test::BM_Template[std::string, std::string]";
  std::string brackets_and_escaped_types_output =
      "examples/google_benchmark/"
      "template_bench.hpp::test::BM_Template[std\\:\\:string, std\\:\\:string]";

  EXPECT_EQ(sanitize_bench_args(brackets_and_escaped_types_input),
            brackets_and_escaped_types_output);

  std::string brackets_and_multiple_types_input =
      "examples/google_benchmark/"
      "template_bench.hpp::test::BM_Template[std::string, int, double]";
  std::string brackets_and_multiple_types_output =
      "examples/google_benchmark/"
      "template_bench.hpp::test::BM_Template[std\\:\\:string, int, double]";

  EXPECT_EQ(sanitize_bench_args(brackets_and_multiple_types_input),
            brackets_and_multiple_types_output);
}
