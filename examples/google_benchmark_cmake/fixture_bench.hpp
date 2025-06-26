// See docs:
// https://github.com/google/benchmark/blob/main/docs/user_guide.md#fixtures

#ifndef FIXTURE_BENCH_HPP
#define FIXTURE_BENCH_HPP

#include <benchmark/benchmark.h>

namespace example_namespace {

class MyFixture : public benchmark::Fixture {
 public:
  void SetUp(::benchmark::State &state) { (void)state; }

  void TearDown(::benchmark::State &state) { (void)state; }
};
BENCHMARK_F(MyFixture, FooTest)(benchmark::State &st) {
  for (auto _ : st) {
  }
}
BENCHMARK_DEFINE_F(MyFixture, BarTest)(benchmark::State &st) {
  for (auto _ : st) {
  }
}
BENCHMARK_REGISTER_F(MyFixture, BarTest);

//
//

template <typename T>
class MyTemplatedFixture : public benchmark::Fixture {};
BENCHMARK_TEMPLATE_F(MyTemplatedFixture, IntTest, int)(benchmark::State &st) {
  for (auto _ : st) {
  }
}
BENCHMARK_TEMPLATE_DEFINE_F(MyTemplatedFixture, DoubleTest, double)
(benchmark::State &st) {
  for (auto _ : st) {
  }
}
BENCHMARK_REGISTER_F(MyTemplatedFixture, DoubleTest);

//
//

template <typename T>
class MyTemplate1 : public benchmark::Fixture {};
BENCHMARK_TEMPLATE1_DEFINE_F(MyTemplate1, TestA, int)(benchmark::State &st) {
  for (auto _ : st) {
  }
}
BENCHMARK_REGISTER_F(MyTemplate1, TestA);

//
//

template <typename T, typename U>
class MyTemplate2 : public benchmark::Fixture {};
BENCHMARK_TEMPLATE2_DEFINE_F(MyTemplate2, TestB, int, double)
(benchmark::State &st) {
  for (auto _ : st) {
  }
}
BENCHMARK_REGISTER_F(MyTemplate2, TestB);

}  // namespace example_namespace

#endif
