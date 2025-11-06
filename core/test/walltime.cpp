#include "walltime.h"

#include <gtest/gtest.h>

#include <cmath>

namespace codspeed {

TEST(WalltimeStatsTest, BasicStatsComputation) {
  // Test with a simple benchmark with consistent iterations
  RawWalltimeBenchmark raw_benchmark;
  raw_benchmark.name = "test_benchmark";
  raw_benchmark.uri = "test_file.cpp";

  // 5 rounds, each with 10 iterations
  // Total round times: 1000ns, 2000ns, 3000ns, 4000ns, 6000ns
  // Per-iteration times: 100ns, 200ns, 300ns, 400ns, 600ns
  // This creates a right-skewed distribution where mean != median
  std::vector<double> round_times = {
      6000.0, 3000.0, 1000.0, 2000.0, 4000.0,
  };
  for (double time : round_times) {
    raw_benchmark.iters_per_round.push_back(10);
    raw_benchmark.times_per_round_ns.push_back(time);
  }

  BenchmarkStats stats = compute_benchmark_stats(raw_benchmark);

  // Per-iteration times are: 100, 200, 300, 400, 600
  EXPECT_DOUBLE_EQ(stats.min_ns, 100.0);
  EXPECT_DOUBLE_EQ(stats.max_ns, 600.0);
  EXPECT_DOUBLE_EQ(stats.mean_ns, 320.0);  // (100+200+300+400+600)/5
  EXPECT_DOUBLE_EQ(stats.median_ns,
                   300.0);  // Middle value (different from mean)
  EXPECT_DOUBLE_EQ(stats.q1_ns, 200.0);
  EXPECT_DOUBLE_EQ(stats.q3_ns, 400.0);
  EXPECT_EQ(stats.rounds, 5);

  // Total time: (1000+2000+3000+4000+6000)ns = 16000ns = 0.000016s
  EXPECT_DOUBLE_EQ(stats.total_time, 16000.0 / 1e9);

  // Average iterations per round: (10+10+10+10+10)/5 = 10
  EXPECT_EQ(stats.iter_per_round, 10);

  // Standard deviation: sqrt(variance)
  // variance = [(100-320)^2 + (200-320)^2 + (300-320)^2 + (400-320)^2 +
  // (600-320)^2] / 5
  //          = [48400 + 14400 + 400 + 6400 + 78400] / 5 = 29600
  // stdev = sqrt(29600) ≈ 172.047
  double expected_stdev = std::sqrt(29600.0);
  EXPECT_NEAR(stats.stdev_ns, expected_stdev, 0.01);

  // No outliers in this right-skewed data (600 is not extreme enough)
  EXPECT_EQ(stats.iqr_outlier_rounds, 0);
  EXPECT_EQ(stats.stdev_outlier_rounds, 0);
}

TEST(WalltimeStatsTest, StdevOutlierDetection) {
  // Test standard deviation outlier detection (3-sigma rule)
  RawWalltimeBenchmark raw_benchmark;
  raw_benchmark.name = "stdev_outlier_benchmark";
  raw_benchmark.uri = "test_file.cpp";

  // Create data with a clear outlier
  std::vector<double> round_times = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                                     1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                                     1.0, 1.0, 1.0, 1.0, 50.0};
  for (double time : round_times) {
    raw_benchmark.iters_per_round.push_back(1);
    raw_benchmark.times_per_round_ns.push_back(time);
  }

  BenchmarkStats stats = compute_benchmark_stats(raw_benchmark);

  EXPECT_NEAR(stats.mean_ns, 3.8823529, 0.001);

  EXPECT_DOUBLE_EQ(stats.median_ns, 1.0);

  EXPECT_NEAR(stats.stdev_ns, 11.53, 0.001);

  EXPECT_EQ(stats.stdev_outlier_rounds, 1);
}

TEST(WalltimeStatsTest, IQROutlierDetection) {
  // Test IQR outlier detection (1.5*IQR rule)
  RawWalltimeBenchmark raw_benchmark;
  raw_benchmark.name = "iqr_outlier_benchmark";
  raw_benchmark.uri = "test_file.cpp";

  // Create data with a clear outlier
  // Per-iteration times: 100, 110, 120, 130, 140, 500 (ns)
  std::vector<double> values = {100, 110, 120, 130, 140, 500};
  for (double val : values) {
    raw_benchmark.iters_per_round.push_back(1);
    raw_benchmark.times_per_round_ns.push_back(val);
  }

  BenchmarkStats stats = compute_benchmark_stats(raw_benchmark);

  EXPECT_EQ(stats.q1_ns, 112.5);
  EXPECT_EQ(stats.q3_ns, 137.5);
  // IQR = 137.5 - 112.5 = 25
  // Lower bound: 112.5 - 1.5*25 = 75
  // Upper bound: 137.5 + 1.5*25 = 175
  // Outlier: 500 (> 175)
  EXPECT_EQ(stats.iqr_outlier_rounds, 1);
}

TEST(WalltimeStatsTest, SingleRound) {
  // Test with a single round
  RawWalltimeBenchmark raw_benchmark;
  raw_benchmark.name = "single_round_benchmark";
  raw_benchmark.uri = "test_file.cpp";

  raw_benchmark.iters_per_round.push_back(5);
  raw_benchmark.times_per_round_ns.push_back(500.0);  // 100ns per iter

  BenchmarkStats stats = compute_benchmark_stats(raw_benchmark);

  // With a single value, all stats should be the same
  EXPECT_DOUBLE_EQ(stats.min_ns, 100.0);
  EXPECT_DOUBLE_EQ(stats.max_ns, 100.0);
  EXPECT_DOUBLE_EQ(stats.mean_ns, 100.0);
  EXPECT_DOUBLE_EQ(stats.median_ns, 100.0);
  EXPECT_DOUBLE_EQ(stats.q1_ns, 100.0);
  EXPECT_DOUBLE_EQ(stats.q3_ns, 100.0);
  EXPECT_DOUBLE_EQ(stats.stdev_ns, 0.0);
  EXPECT_EQ(stats.rounds, 1);
  EXPECT_EQ(stats.iqr_outlier_rounds, 0);
  EXPECT_EQ(stats.stdev_outlier_rounds, 0);

  // Total time: 500ns = 0.0000005s
  EXPECT_DOUBLE_EQ(stats.total_time, 500.0 / 1e9);
}

TEST(WalltimeStatsTest, QantileComputation) {
  // Test quantile computation with a specific dataset
  RawWalltimeBenchmark raw_benchmark;
  raw_benchmark.name = "quantile_test";
  raw_benchmark.uri = "test_file.cpp";

  // Per-iteration times: 10, 20, 30, 40, 50, 60, 70, 80, 90 (ns)
  std::vector<double> round_times = {10.0, 20.0, 30.0, 40.0, 50.0,
                                     60.0, 70.0, 80.0, 90.0};
  for (double time : round_times) {
    raw_benchmark.iters_per_round.push_back(1);
    raw_benchmark.times_per_round_ns.push_back(time);
  }

  BenchmarkStats stats = compute_benchmark_stats(raw_benchmark);

  // With 9 values (indices 0-8):
  // Q1 (0.25): pos = 0.25 * 8 = 2.0 -> value at index 2 = 30
  // Median (0.5): pos = 0.5 * 8 = 4.0 -> value at index 4 = 50
  // Q3 (0.75): pos = 0.75 * 8 = 6.0 -> value at index 6 = 70
  EXPECT_DOUBLE_EQ(stats.q1_ns, 30.0);
  EXPECT_DOUBLE_EQ(stats.median_ns, 50.0);
  EXPECT_DOUBLE_EQ(stats.q3_ns, 70.0);
}

TEST(WalltimeStatsTest, QuantileInterpolation) {
  // Test quantile computation with a specific dataset
  RawWalltimeBenchmark raw_benchmark;
  raw_benchmark.name = "quantile_test";
  raw_benchmark.uri = "test_file.cpp";

  std::vector<double> round_times = {10.0, 20.0, 30.0, 40.0,
                                     50.0, 60.0, 70.0, 80.0};
  for (double time : round_times) {
    raw_benchmark.iters_per_round.push_back(1);
    raw_benchmark.times_per_round_ns.push_back(time);
  }

  BenchmarkStats stats = compute_benchmark_stats(raw_benchmark);

  EXPECT_DOUBLE_EQ(stats.q1_ns, 27.5);
  EXPECT_DOUBLE_EQ(stats.median_ns, 45.0);
  EXPECT_DOUBLE_EQ(stats.q3_ns, 62.5);
}

}  // namespace codspeed
