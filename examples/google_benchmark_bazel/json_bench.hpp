#pragma once

#include <benchmark/benchmark.h>
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

// Benchmark JSON parsing from string
static void BM_JsonParse(benchmark::State& state) {
  std::string json_str = R"({
    "name": "John Doe",
    "age": 30,
    "city": "New York",
    "hobbies": ["reading", "gaming", "coding"],
    "address": {
      "street": "123 Main St",
      "zip": "10001"
    }
  })";

  for (auto _ : state) {
    json j = json::parse(json_str);
    benchmark::DoNotOptimize(j);
    benchmark::ClobberMemory();
  }
}
BENCHMARK(BM_JsonParse);

// Benchmark JSON serialization to string
static void BM_JsonSerialize(benchmark::State& state) {
  json j = {
    {"name", "John Doe"},
    {"age", 30},
    {"city", "New York"},
    {"hobbies", {"reading", "gaming", "coding"}},
    {"address", {
      {"street", "123 Main St"},
      {"zip", "10001"}
    }}
  };

  for (auto _ : state) {
    std::string s = j.dump();
    benchmark::DoNotOptimize(s);
    benchmark::ClobberMemory();
  }
}
BENCHMARK(BM_JsonSerialize);

// Benchmark JSON object creation
static void BM_JsonCreate(benchmark::State& state) {
  for (auto _ : state) {
    json j = {
      {"name", "John Doe"},
      {"age", 30},
      {"city", "New York"},
      {"hobbies", {"reading", "gaming", "coding"}},
      {"address", {
        {"street", "123 Main St"},
        {"zip", "10001"}
      }}
    };
    benchmark::DoNotOptimize(j);
    benchmark::ClobberMemory();
  }
}
BENCHMARK(BM_JsonCreate);

// Benchmark JSON array operations
static void BM_JsonArrayAppend(benchmark::State& state) {
  for (auto _ : state) {
    json j = json::array();
    for (int i = 0; i < state.range(0); ++i) {
      j.push_back(i);
    }
    benchmark::DoNotOptimize(j);
    benchmark::ClobberMemory();
  }
}
BENCHMARK(BM_JsonArrayAppend)->Range(8, 8 << 10);
