cmake_minimum_required(VERSION 3.12)
include(FetchContent)

project(codspeed_process_isolation_test VERSION 0.0.0 LANGUAGES CXX)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Werror)
endif()

set(CMAKE_BUILD_TYPE RelWithDebInfo)

# Keep ctest limited to the isolation test.
set(BENCHMARK_ENABLE_TESTING
    OFF
    CACHE BOOL
    "Enable testing of the benchmark library."
    FORCE
)
set(BENCHMARK_ENABLE_GTEST_TESTS
    OFF
    CACHE BOOL
    "Enable testing of the benchmark library."
    FORCE
)

FetchContent_Declare(
    google_benchmark
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/../../google_benchmark
)
FetchContent_MakeAvailable(google_benchmark)

find_package(Threads REQUIRED)
add_executable(process_isolation_fixture main.cpp)
target_link_libraries(
    process_isolation_fixture
    PRIVATE benchmark::benchmark Threads::Threads
)

# Process isolation only exists in the analysis modes.
enable_testing()
add_test(
    NAME process_isolation
    COMMAND
        ${CMAKE_CURRENT_SOURCE_DIR}/run_test.sh
        $<TARGET_FILE:process_isolation_fixture>
)
