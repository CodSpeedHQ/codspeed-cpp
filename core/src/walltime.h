#ifndef CODSPEED_WALLTIME_INTERNAL_H
#define CODSPEED_WALLTIME_INTERNAL_H

#include <cstdint>
#include <vector>

#include "codspeed.h"

namespace codspeed {

// Times are per iteration
struct BenchmarkStats {
  double min_ns;
  double max_ns;
  double mean_ns;
  double stdev_ns;
  double q1_ns;
  double median_ns;
  double q3_ns;
  uint64_t rounds;
  double total_time;
  uint64_t iqr_outlier_rounds;
  uint64_t stdev_outlier_rounds;
  uint64_t iter_per_round;
  uint64_t warmup_iters;

  BenchmarkStats(double min_ns = 0.0, double max_ns = 0.0, double mean_ns = 0.0,
                 double stdev_ns = 0.0, double q1_ns = 0.0,
                 double median_ns = 0.0, double q3_ns = 0.0,
                 uint64_t rounds = 0, double total_time = 0.0,
                 uint64_t iqr_outlier_rounds = 0,
                 uint64_t stdev_outlier_rounds = 0, uint64_t iter_per_round = 0,
                 uint64_t warmup_iters = 0)
      : min_ns(min_ns),
        max_ns(max_ns),
        mean_ns(mean_ns),
        stdev_ns(stdev_ns),
        q1_ns(q1_ns),
        median_ns(median_ns),
        q3_ns(q3_ns),
        rounds(rounds),
        total_time(total_time),
        iqr_outlier_rounds(iqr_outlier_rounds),
        stdev_outlier_rounds(stdev_outlier_rounds),
        iter_per_round(iter_per_round),
        warmup_iters(warmup_iters) {}
};

// Compute benchmark statistics from raw walltime data
BenchmarkStats compute_benchmark_stats(
    const RawWalltimeBenchmark &raw_benchmark);

}  // namespace codspeed

#endif  // CODSPEED_WALLTIME_INTERNAL_H
