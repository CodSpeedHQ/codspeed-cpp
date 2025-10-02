#ifndef HELPER_HPP
#define HELPER_HPP

#ifdef _MSC_VER
#define NOINLINE __declspec(noinline)
#else
#define NOINLINE __attribute__((noinline))
#endif

#include <benchmark/benchmark.h>

#include <cstdint>
#include <numeric>
#include <vector>

NOINLINE static uint64_t recursive_fib(int n) {
  if (n <= 1) return n;
  return recursive_fib(n - 1) + recursive_fib(n - 2);
}

NOINLINE static uint64_t expensive_operation() {
  // Large memory allocation
  std::vector<uint64_t> data(1024 * 1024, 42);  // 8 MiB allocation

  // Expensive recursive computation that will dominate flamegraph
  uint64_t fib_result = recursive_fib(30);

  // More expensive work
  uint64_t sum = std::accumulate(data.begin(), data.end(), uint64_t(0));
  benchmark::DoNotOptimize(sum);
  benchmark::DoNotOptimize(fib_result);

  return sum + fib_result;
}

#endif  // HELPER_HPP
