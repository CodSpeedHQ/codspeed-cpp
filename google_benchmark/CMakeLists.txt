cmake_minimum_required(VERSION 3.12)
include(FetchContent)

project(codspeed_google_benchmark_compat VERSION 0.0.0 LANGUAGES CXX)

FetchContent_Declare(
  google_benchmark
  GIT_REPOSITORY https://github.com/google/benchmark.git
  GIT_TAG        12235e24652fc7f809373e7c11a5f73c5763fc4c # v1.9.0
)
FetchContent_MakeAvailable(google_benchmark)

add_library(codspeed_picobench_compat)

