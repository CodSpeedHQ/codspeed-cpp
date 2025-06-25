cmake_minimum_required(VERSION 3.12)
include(FetchContent)

project(codspeed_google_benchmark_example VERSION 0.0.0 LANGUAGES CXX)

# Treat warnings as errors
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Werror)
elseif(MSVC)
    # Set C++17 standard
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)

    add_compile_options(
        /W4
        /WX
        /wd5051 # Ignore [[maybe_unused]] C++17 warnings from Google Benchmark
        /wd4038 # Ignore STL <filesystem> C++17 warnings
    )
endif()

# On the small benchmarks of this repo, most of the benches will be optimized out, but this is expected.
set(CMAKE_BUILD_TYPE RelWithDebInfo)

# Disable Google Benchmark tests for examples
set(BENCHMARK_ENABLE_GTEST_TESTS
    OFF
    CACHE BOOL
    "Enable testing of the benchmark library."
    FORCE
)

FetchContent_Declare(
    google_benchmark
    SOURCE_DIR
    ${CMAKE_SOURCE_DIR}/../../google_benchmark
)

FetchContent_MakeAvailable(google_benchmark)

add_executable(benchmark_example main.cpp)

target_link_libraries(benchmark_example benchmark::benchmark)
