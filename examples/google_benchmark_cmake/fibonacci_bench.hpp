#pragma once

#include <benchmark/benchmark.h>

static uint64_t fibonacci_recursive(int n) {
  if (n <= 1) return n;
  return fibonacci_recursive(n - 1) + fibonacci_recursive(n - 2);
}

static void BM_FibonacciRecursive(benchmark::State& state) {
  int n = static_cast<int>(state.range(0));
  for (auto _ : state) {
    uint64_t result = fibonacci_recursive(n);
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_FibonacciRecursive)->Arg(35)->MinTime(5);

static uint64_t fibonacci_iterative(int n) {
  if (n <= 1) return n;

  uint64_t a = 0, b = 1;
  for (int i = 2; i <= n; ++i) {
    uint64_t c = a + b;
    a = b;
    b = c;
  }
  return b;
}

static void BM_FibonacciIterative(benchmark::State& state) {
  int n = static_cast<int>(state.range(0));
  for (auto _ : state) {
    uint64_t result = fibonacci_iterative(n);
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_FibonacciIterative)->Arg(50);
