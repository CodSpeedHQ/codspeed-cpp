cmake_minimum_required(VERSION 3.12)
include(FetchContent)

project(
  codspeed_picobench_compat
  VERSION 0.0.0
  LANGUAGES CXX)

FetchContent_Declare(
  picobench
  GIT_REPOSITORY https://github.com/iboB/picobench.git
  GIT_TAG 7b5de5ab7dad0a9d6627e63c8757c9c4f0c6b1b3 # v2.07
)

FetchContent_MakeAvailable(picobench)

add_library(codspeed_picobench_compat INTERFACE)
add_library(codspeed_picobench_compat::codspeed_picobench_compat ALIAS
            codspeed_picobench_compat)
target_include_directories(codspeed_picobench_compat
                           INTERFACE ${picobench_SOURCE_DIR}/include include)

add_subdirectory(examples)
