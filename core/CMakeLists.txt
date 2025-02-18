cmake_minimum_required(VERSION 3.10)
project(
  codspeed
  VERSION 1.0
  LANGUAGES CXX)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the include directory
include_directories(include)

# Add the library
add_library(codspeed src/codspeed.cpp)

# Specify the include directories for users of the library
target_include_directories(
  codspeed PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>)
