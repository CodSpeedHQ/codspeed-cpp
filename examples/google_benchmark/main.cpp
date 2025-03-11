#include <benchmark/benchmark.h>
#include <cstring>

// Passing Arbitrary Arguments to a Benchmark
// See:
// https://github.com/google/benchmark/blob/main/docs/user_guide.md#passing-arbitrary-arguments-to-a-benchmark
template <class... Args>
void BM_takes_args(benchmark::State &state, Args &&...args) {
  auto args_tuple = std::make_tuple(std::move(args)...);
  for (auto _ : state) {
  }
}
BENCHMARK_CAPTURE(BM_takes_args, int_string_test, 42, std::string("abc"));
BENCHMARK_CAPTURE(BM_takes_args, int_test, 42, 43);

// Benchmark template
//
template <class T> void BM_VectorPushBack(benchmark::State &state) {
  std::vector<T> v;
  for (auto _ : state) {
    v.push_back(T());
  }
}
BENCHMARK_TEMPLATE(BM_VectorPushBack, int);
BENCHMARK_TEMPLATE(BM_VectorPushBack, std::string);

template <typename T> void BM_Template1(benchmark::State &state) {
  T val = T();
  for (auto _ : state) {
    benchmark::DoNotOptimize(val++);
  }
}
BENCHMARK_TEMPLATE1(BM_Template1, int);

template <typename T, typename U> void BM_Template2(benchmark::State &state) {
  T t = T();
  U u = U();
  for (auto _ : state) {
    benchmark::DoNotOptimize(t + u);
  }
}
BENCHMARK_TEMPLATE2(BM_Template2, int, double);

// Benchmark template capture
//
template <typename T, class... ExtraArgs>
void BM_takes_args(benchmark::State &state, ExtraArgs &&...extra_args) {
  auto args_tuple = std::make_tuple(std::move(extra_args)...);
  for (auto _ : state) {
  }
}
BENCHMARK_TEMPLATE1_CAPTURE(BM_takes_args, void, int_string_test, 42,
                            std::string("abc"));

BENCHMARK_TEMPLATE2_CAPTURE(BM_takes_args, int, double, two_type_test, 42,
                            std::string("abc"));

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

static void BM_memcpy(benchmark::State &state) {
  char *src = new char[state.range(0)];
  char *dst = new char[state.range(0)];
  memset(src, 'x', state.range(0));
  for (auto _ : state)
    memcpy(dst, src, state.range(0));
  delete[] src;
  delete[] dst;
}

BENCHMARK(BM_memcpy)->Range(8, 8 << 10);

BENCHMARK_MAIN();
