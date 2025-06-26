#pragma once

#include <benchmark/benchmark.h>

#include <chrono>
#include <thread>

// Note: `std::this_thread::sleep_for` and other methods rely on the kernel's
// scheduling and may not be precise for very short durations (<1ms). By using
// a busy-wait loop, we can achieve more precise timing at the cost of CPU
// usage.
//
// See COD-1155 for more information.
static inline void busy_sleep_for(std::chrono::nanoseconds duration) {
  auto start = std::chrono::steady_clock::now();
  while ((std::chrono::steady_clock::now() - start) < duration) {
    // Busy-wait
  }
}

static void BM_sleep_1ns(benchmark::State& state) {
  for (auto _ : state) {
    busy_sleep_for(std::chrono::nanoseconds(1));
  }
}
BENCHMARK(BM_sleep_1ns);

static void BM_sleep_10ns(benchmark::State& state) {
  for (auto _ : state) {
    busy_sleep_for(std::chrono::nanoseconds(10));
  }
}
BENCHMARK(BM_sleep_10ns);

static void BM_sleep_50ns(benchmark::State& state) {
  for (auto _ : state) {
    busy_sleep_for(std::chrono::nanoseconds(50));
  }
}
BENCHMARK(BM_sleep_50ns);

static void BM_sleep_100ns(benchmark::State& state) {
  for (auto _ : state) {
    busy_sleep_for(std::chrono::nanoseconds(100));
  }
}
BENCHMARK(BM_sleep_100ns);

static void BM_sleep_1us(benchmark::State& state) {
  for (auto _ : state) {
    busy_sleep_for(std::chrono::microseconds(1));
  }
}
BENCHMARK(BM_sleep_1us);

static void BM_sleep_10us(benchmark::State& state) {
  for (auto _ : state) {
    busy_sleep_for(std::chrono::microseconds(10));
  }
}
BENCHMARK(BM_sleep_10us);

static void BM_sleep_50us(benchmark::State& state) {
  for (auto _ : state) {
    busy_sleep_for(std::chrono::microseconds(50));
  }
}
BENCHMARK(BM_sleep_50us);

static void BM_sleep_100us(benchmark::State& state) {
  for (auto _ : state) {
    busy_sleep_for(std::chrono::microseconds(100));
  }
}
BENCHMARK(BM_sleep_100us);

static void BM_sleep_1ms(benchmark::State& state) {
  for (auto _ : state) {
    busy_sleep_for(std::chrono::milliseconds(1));
  }
}
BENCHMARK(BM_sleep_1ms);

static void BM_sleep_10ms(benchmark::State& state) {
  for (auto _ : state) {
    busy_sleep_for(std::chrono::milliseconds(10));
  }
}
BENCHMARK(BM_sleep_10ms);

static void BM_sleep_50ms(benchmark::State& state) {
  for (auto _ : state) {
    busy_sleep_for(std::chrono::milliseconds(50));
  }
}
BENCHMARK(BM_sleep_50ms);

static void BM_sleep_100ms(benchmark::State& state) {
  for (auto _ : state) {
    busy_sleep_for(std::chrono::milliseconds(100));
  }
}
BENCHMARK(BM_sleep_100ms);
