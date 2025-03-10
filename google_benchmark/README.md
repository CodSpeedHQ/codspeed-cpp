<div align="center">
<h1>codspeed-google-benchmark</h1>

[![CI](https://github.com/CodSpeedHQ/codspeed-cpp/actions/workflows/ci.yml/badge.svg?branch=main)](https://github.com/CodSpeedHQ/codspeed-cpp/actions/workflows/ci.yml)
[![Discord](https://img.shields.io/badge/chat%20on-discord-7289da.svg)](https://discord.com/invite/MxpaCfKSqF)
[![CodSpeed Badge](https://img.shields.io/endpoint?url=https://codspeed.io/badge.json)](https://codspeed.io/CodSpeedHQ/codspeed-cpp)

Google benchmark compatibility layer for CodSpeed

</div>

## Installation

The recommended way to use CodSpeed's `google_benchmark` is through `cmake`. Add
the following to your `CMakeLists.txt`:

```cmake title="CMakeLists.txt"
cmake_minimum_required(VERSION 3.12)
include(FetchContent)

project(my_codspeed_project VERSION 0.0.0 LANGUAGES CXX)

set(BENCHMARK_DOWNLOAD_DEPENDENCIES ON)

FetchContent_Declare(
    google_benchmark
    GIT_REPOSITORY https://github.com/CodSpeedHQ/codspeed-cpp
    GIT_TAG v1.0.0
    SOURCE_SUBDIR
    google_benchmark
)

FetchContent_MakeAvailable(google_benchmark)

# Declare your benchmark executable and its sources here
add_executable(my_benchmark_executable main.cpp)

# Link your executable against the `benchmark::benchmark` library
target_link_libraries(my_benchmark_example benchmark::benchmark)
```

## Usage

### Creating benchmark

Here is an example benchmark, follow the [google benchmark documentation](https://github.com/google/benchmark/blob/main/docs/user_guide.md) for more advanced usage.

```cpp title="main.cpp"
// Define the function under test
static void BM_StringCopy(benchmark::State &state) {
  std::string x = "hello";

  // Google benchmark relies on state.begin() and state.end() to run the benchmark and count iterations
  for (auto _ : state) {
    std::string copy(x);
  }
}
// Register the benchmarked to be called by the executable
BENCHMARK(BM_StringCopy);

static void BM_memcpy(benchmark::State &state) {
  char *src = new char[state.range(0)];
  char *dst = new char[state.range(0)];
  memset(src, 'x', state.range(0));
  for (auto _ : state)
    memcpy(dst, src, state.range(0));
  delete[] src;
  delete[] dst;
}

BENCHMARK(BM_memcpy)->Range(8, 8 << 10);
```

### Compiling and running benchmarks

To build and run the benchmark executable

```
$ mkdir build
$ cd build
$ cmake -DCODSPEED_MODE=instrumentation ..
    ...
    -- Configuring done (8.9s)
    -- Generating done (0.1s)
    -- Build files have been written to: /home/user/my-project/build
$ make -j
    ...
    [ 98%] Built target reporter_output_test
    [100%] Linking CXX executable benchmark_gtest
    [100%] Built target benchmark_gtest
$ ./my-bench
    NOTICE: codspeed is enabled, but no performance measurement will be made since it's running in an unknown environment.
    Checked: main.cpp::BM_rand_vector
    Checked: main.cpp::BM_StringCopy
    Checked: main.cpp::BM_memcpy[8]
    Checked: main.cpp::BM_memcpy[64]
    Checked: main.cpp::BM_memcpy[512]
    Checked: main.cpp::BM_memcpy[4096]
    Checked: main.cpp::BM_memcpy[8192]
```

For more information, please checkout the [codspeed documentation](https://docs.codspeed.io/benchmarks/cpp)
