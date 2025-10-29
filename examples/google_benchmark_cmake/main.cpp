#include <benchmark/benchmark.h>

#include <cstring>

#include "fibonacci_bench.hpp"
#include "fixture_bench.hpp"
#include "json_bench.hpp"
#include "multithread_bench.hpp"
#include "pause_timing_bench.hpp"
#include "sleep_bench.hpp"
#include "template_bench.hpp"

template <class... Args>
void BM_Capture(benchmark::State &state, Args &&...args) {
  auto args_tuple = std::make_tuple(std::move(args)...);
  for (auto _ : state) {
    benchmark::DoNotOptimize(args_tuple);
    benchmark::ClobberMemory();
  }
}
BENCHMARK_CAPTURE(BM_Capture, int_string_test, 42, std::string("abc"));
BENCHMARK_CAPTURE(BM_Capture, int_test, 42, 43);

// Function to benchmark
static void BM_rand_vector(benchmark::State &state) {
  std::vector<int> v;
  for (auto _ : state) {
    std::string empty_string;
    benchmark::DoNotOptimize(empty_string);
    benchmark::ClobberMemory();
  }
}
// Register the function as a benchmark
BENCHMARK(BM_rand_vector);

// Function to benchmark with a parameter
static void BM_StringCopy(benchmark::State &state) {
  std::string x = "hello";
  for (auto _ : state) {
    std::string copy(x);
    benchmark::DoNotOptimize(copy);
    benchmark::ClobberMemory();
  }
}
// Register the function as a benchmark
BENCHMARK(BM_StringCopy);

static void BM_memcpy(benchmark::State &state) {
  char *src = new char[state.range(0)];
  char *dst = new char[state.range(0)];
  memset(src, 'x', state.range(0));
  for (auto _ : state) {
    memcpy(dst, src, state.range(0));
    benchmark::DoNotOptimize(dst);
    benchmark::ClobberMemory();
  }
  delete[] src;
  delete[] dst;
}

BENCHMARK(BM_memcpy)->Range(8, 8 << 10);

BENCHMARK_MAIN();
