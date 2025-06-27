#pragma once

#include <benchmark/benchmark.h>

#include <chrono>
#include <thread>

static void BM_sleep_1ns(benchmark::State& state) {
  for (auto _ : state) {
    std::this_thread::sleep_for(std::chrono::nanoseconds(1));
  }
}
BENCHMARK(BM_sleep_1ns);

static void BM_sleep_100ns(benchmark::State& state) {
  for (auto _ : state) {
    std::this_thread::sleep_for(std::chrono::nanoseconds(100));
  }
}
BENCHMARK(BM_sleep_100ns);

static void BM_sleep_1us(benchmark::State& state) {
  for (auto _ : state) {
    std::this_thread::sleep_for(std::chrono::microseconds(1));
  }
}
BENCHMARK(BM_sleep_1us);

static void BM_sleep_100us(benchmark::State& state) {
  for (auto _ : state) {
    std::this_thread::sleep_for(std::chrono::microseconds(100));
  }
}
BENCHMARK(BM_sleep_100us);

static void BM_sleep_1ms(benchmark::State& state) {
  for (auto _ : state) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
}
BENCHMARK(BM_sleep_1ms);

static void BM_sleep_10ms(benchmark::State& state) {
  for (auto _ : state) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
}
BENCHMARK(BM_sleep_10ms);

static void BM_sleep_50ms(benchmark::State& state) {
  for (auto _ : state) {
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }
}
BENCHMARK(BM_sleep_50ms);

static void BM_sleep_100ms(benchmark::State& state) {
  for (auto _ : state) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}
BENCHMARK(BM_sleep_100ms);
