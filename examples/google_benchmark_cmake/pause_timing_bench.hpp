#pragma once

#include <benchmark/benchmark.h>

#include <chrono>
#include <thread>

#include "helper.hpp"

NOINLINE static uint64_t actual_work() {
  std::this_thread::sleep_for(std::chrono::milliseconds(1));
  return 42;
}

static void BM_large_setup_teardown(benchmark::State& state) {
  for (auto _ : state) {
    state.PauseTiming();
    benchmark::DoNotOptimize(expensive_operation());
    state.ResumeTiming();

    benchmark::DoNotOptimize(actual_work());

    state.PauseTiming();
    benchmark::DoNotOptimize(expensive_operation());
    state.ResumeTiming();
  }
}
// IMPORTANT: Use fixed iterations, otherwise we'll run for 10+ minutes
BENCHMARK(BM_large_setup_teardown)->Iterations(100);

static void BM_large_setup(benchmark::State& state) {
  for (auto _ : state) {
    state.PauseTiming();
    benchmark::DoNotOptimize(expensive_operation());
    state.ResumeTiming();

    benchmark::DoNotOptimize(actual_work());
  }
}
BENCHMARK(BM_large_setup);
