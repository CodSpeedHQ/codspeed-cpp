cmake_minimum_required(VERSION 3.12)
include(FetchContent)

project(codspeed_picobench_compat VERSION 0.0.0 LANGUAGES CXX)

set(BENCHMARK_DOWNLOAD_DEPENDENCIES ON)

FetchContent_Declare(
    google_benchmark
    SOURCE_DIR
    ${CMAKE_SOURCE_DIR}/../../google_benchmark
)

FetchContent_MakeAvailable(google_benchmark)

add_executable(benchmark_example main.cpp)

target_link_libraries(benchmark_example benchmark::benchmark)
