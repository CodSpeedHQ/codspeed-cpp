#include <benchmark/benchmark.h>
#include <cstring>

// Function to benchmark
static void BM_rand_vector(benchmark::State &state) {
  std::vector<int> v;
  for (auto _ : state) {
    std::string empty_string;
  }
}
// Register the function as a benchmark
BENCHMARK(BM_rand_vector);

// Function to benchmark with a parameter
static void BM_StringCopy(benchmark::State &state) {
  std::string x = "hello";
  for (auto _ : state) {
    std::string copy(x);
  }
}
// Register the function as a benchmark
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

BENCHMARK_MAIN();
