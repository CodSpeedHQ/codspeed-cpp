#pragma once

#include <benchmark/benchmark.h>

#include <atomic>
#include <thread>

#include "helper.hpp"

// Simple multithreaded benchmark: spawn thread, do work, join
static void BM_SimpleMultithread(benchmark::State& state) {
  for (auto _ : state) {
    state.PauseTiming();
    std::atomic<bool> work_start{false};
    std::atomic<bool> work_done{false};

    std::thread worker([&]() {
      // Wait for the signal to start
      while (!work_start.load()) {
        std::this_thread::yield();
      }

      // Do some CPU work
      benchmark::DoNotOptimize(expensive_operation());
      work_done.store(true);
    });
    state.ResumeTiming();

    // Signal the worker to start
    work_start.store(true);

    // Wait for worker to complete
    while (!work_done.load()) {
      std::this_thread::yield();
    }

    worker.join();
  }
}

BENCHMARK(BM_SimpleMultithread);
