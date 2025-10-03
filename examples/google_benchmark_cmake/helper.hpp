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
  // Expensive recursive compuation that will dominate flamegraph
  return 42 + recursive_fib(30);
}

#endif  // HELPER_HPP
