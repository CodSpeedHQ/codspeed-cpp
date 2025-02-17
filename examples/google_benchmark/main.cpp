#include <benchmark/benchmark.h>

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

BENCHMARK_MAIN();
