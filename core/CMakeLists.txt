cmake_minimum_required(VERSION 3.10)

set(CODSPEED_VERSION 1.0.0)

project(codspeed VERSION ${CODSPEED_VERSION} LANGUAGES CXX)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the include directory
include_directories(include)

# Add the library
add_library(codspeed src/codspeed.cpp src/walltime.cpp src/uri.cpp)

# Version
add_compile_definitions(CODSPEED_VERSION="${CODSPEED_VERSION}")

# Disable valgrind compilation errors
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    # Disable the old-style-cast warning for the specific target
    target_compile_options(codspeed PRIVATE -Wno-old-style-cast)
endif()

# Specify the include directories for users of the library
target_include_directories(
    codspeed
    PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
)
