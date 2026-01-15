#pragma once

#include <benchmark/benchmark.h>

#include <string>
#include <vector>

// Run-length encoding: compress consecutive repeated characters
// Example: "aaabbbccc" -> "3a3b3c"
// NOTE: Intentionally inefficient - no pre-allocation to show multiple
// allocations
static std::string rle_encode(const std::string& input) {
  if (input.empty()) return "";

  std::string result;  // No reserve - will trigger multiple reallocations

  char current = input[0];
  size_t count = 1;

  for (size_t i = 1; i < input.size(); ++i) {
    if (input[i] == current) {
      count++;
    } else {
      // Create intermediate strings for each run
      std::string count_str = std::to_string(count);
      std::string run_encoded = count_str + current;
      result += run_encoded;  // Concatenation causes reallocations
      current = input[i];
      count = 1;
    }
  }

  // Final run
  std::string count_str = std::to_string(count);
  std::string final_run = count_str + current;
  result += final_run;

  return result;
}

// Run-length decoding: decompress RLE encoded string
// Example: "3a3b3c" -> "aaabbbccc"
static std::string rle_decode(const std::string& input) {
  std::string result;
  size_t i = 0;

  while (i < input.size()) {
    // Parse the count
    size_t count = 0;
    while (i < input.size() && std::isdigit(input[i])) {
      count = count * 10 + (input[i] - '0');
      i++;
    }

    // Get the character
    if (i < input.size()) {
      char ch = input[i];
      result.append(count, ch);
      i++;
    }
  }

  return result;
}

// Generate a string with patterns for RLE
static std::string generate_rle_input(size_t size, size_t run_length) {
  std::string result;
  result.reserve(size);

  const std::string chars = "abcdefghijklmnopqrstuvwxyz";
  size_t char_idx = 0;

  while (result.size() < size) {
    size_t count = std::min(run_length, size - result.size());
    result.append(count, chars[char_idx % chars.size()]);
    char_idx++;
  }

  return result;
}

// Benchmark: RLE encoding with small runs (high compression)
static void BM_RLE_Encode_SmallRuns(benchmark::State& state) {
  const size_t input_size = state.range(0);
  std::string input = generate_rle_input(input_size, 3);

  for (auto _ : state) {
    std::string encoded = rle_encode(input);
    benchmark::DoNotOptimize(encoded);
    benchmark::ClobberMemory();
  }

  state.SetBytesProcessed(state.iterations() * input_size);
}
BENCHMARK(BM_RLE_Encode_SmallRuns)
    ->Arg(100)
    ->Arg(1000)
    ->Arg(10000)
    ->Arg(100000);

// Benchmark: RLE encoding with large runs (low compression)
static void BM_RLE_Encode_LargeRuns(benchmark::State& state) {
  const size_t input_size = state.range(0);
  std::string input = generate_rle_input(input_size, 100);

  for (auto _ : state) {
    std::string encoded = rle_encode(input);
    benchmark::DoNotOptimize(encoded);
    benchmark::ClobberMemory();
  }

  state.SetBytesProcessed(state.iterations() * input_size);
}
BENCHMARK(BM_RLE_Encode_LargeRuns)
    ->Arg(100)
    ->Arg(1000)
    ->Arg(10000)
    ->Arg(100000);

// Benchmark: RLE decoding
static void BM_RLE_Decode(benchmark::State& state) {
  const size_t input_size = state.range(0);
  std::string input = generate_rle_input(input_size, 10);
  std::string encoded = rle_encode(input);

  for (auto _ : state) {
    std::string decoded = rle_decode(encoded);
    benchmark::DoNotOptimize(decoded);
    benchmark::ClobberMemory();
  }

  state.SetBytesProcessed(state.iterations() * encoded.size());
}
BENCHMARK(BM_RLE_Decode)->Arg(100)->Arg(1000)->Arg(10000)->Arg(100000);

// Benchmark: Vector allocations (resizing pattern)
static void BM_Vector_PushBack(benchmark::State& state) {
  const size_t count = state.range(0);

  for (auto _ : state) {
    std::vector<int> vec;
    for (size_t i = 0; i < count; ++i) {
      vec.push_back(static_cast<int>(i));
    }
    benchmark::DoNotOptimize(vec);
    benchmark::ClobberMemory();
  }
}
BENCHMARK(BM_Vector_PushBack)->Arg(10)->Arg(100)->Arg(1000)->Arg(10000);

// Benchmark: Vector allocations with reserve (optimized)
static void BM_Vector_Reserve(benchmark::State& state) {
  const size_t count = state.range(0);

  for (auto _ : state) {
    std::vector<int> vec;
    vec.reserve(count);
    for (size_t i = 0; i < count; ++i) {
      vec.push_back(static_cast<int>(i));
    }
    benchmark::DoNotOptimize(vec);
    benchmark::ClobberMemory();
  }
}
BENCHMARK(BM_Vector_Reserve)->Arg(10)->Arg(100)->Arg(1000)->Arg(10000);

// Benchmark: String concatenation (many allocations)
static void BM_String_Concatenation(benchmark::State& state) {
  const size_t count = state.range(0);

  for (auto _ : state) {
    std::string result;
    for (size_t i = 0; i < count; ++i) {
      result += "x";
    }
    benchmark::DoNotOptimize(result);
    benchmark::ClobberMemory();
  }
}
BENCHMARK(BM_String_Concatenation)->Arg(10)->Arg(100)->Arg(1000)->Arg(10000);

// Benchmark: String concatenation with reserve (optimized)
static void BM_String_Reserve(benchmark::State& state) {
  const size_t count = state.range(0);

  for (auto _ : state) {
    std::string result;
    result.reserve(count);
    for (size_t i = 0; i < count; ++i) {
      result += "x";
    }
    benchmark::DoNotOptimize(result);
    benchmark::ClobberMemory();
  }
}
BENCHMARK(BM_String_Reserve)->Arg(10)->Arg(100)->Arg(1000)->Arg(10000);
