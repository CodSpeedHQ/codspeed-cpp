#pragma once

#include <benchmark/benchmark.h>
#include <cstring>
#include <string>

namespace test {
template <class T> void BM_Template(benchmark::State &state) {
  std::vector<T> v;
  for (auto _ : state) {
    v.push_back(T());
  }
}
BENCHMARK_TEMPLATE(BM_Template, int);
BENCHMARK_TEMPLATE(BM_Template, std::string);
}

//
//

template <typename T> void BM_Template1(benchmark::State &state) {
  T val = T();
  for (auto _ : state) {
    benchmark::DoNotOptimize(val++);
  }
}
BENCHMARK_TEMPLATE1(BM_Template1, int);

//
//

template <typename T, typename U> void BM_Template2(benchmark::State &state) {
  T t = T();
  U u = U();
  for (auto _ : state) {
    benchmark::DoNotOptimize(t + u);
  }
}
BENCHMARK_TEMPLATE2(BM_Template2, int, double);

//
//

template <typename T, class... ExtraArgs>
void BM_Template1_Capture(benchmark::State &state, ExtraArgs &&...extra_args) {
  auto args_tuple = std::make_tuple(std::move(extra_args)...);
  for (auto _ : state) {
  }
}
BENCHMARK_TEMPLATE1_CAPTURE(BM_Template1_Capture, void, int_string_test, 42,
                            std::string("abc"));
BENCHMARK_TEMPLATE2_CAPTURE(BM_Template1_Capture, int, double, two_type_test,
                            42, std::string("abc"));
