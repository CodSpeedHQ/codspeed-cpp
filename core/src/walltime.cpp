#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string>

#include "codspeed.h"
#include "utils.h"
#ifdef _WIN32
#include <process.h>
#else
#include <unistd.h>
#endif
#include <vector>

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

struct BenchmarkMetadata {
  std::string name;
  std::string uri;
};

struct CodspeedWalltimeBenchmark {
  BenchmarkMetadata metadata;
  BenchmarkStats stats;
};

static double compute_quantile(const std::vector<double> &sorted_data,
                               double quantile) {
  size_t n = sorted_data.size();
  if (n == 0) return 0.0;

  double pos = quantile * (n - 1);
  size_t k = static_cast<size_t>(pos);
  double d = pos - k;

  if (k + 1 < n) {
    return sorted_data[k] + d * (sorted_data[k + 1] - sorted_data[k]);
  }
  return sorted_data[k];
}

static std::string escape_backslashes(const std::string &input) {
  std::string output;
  for (char c : input) {
    if (c == '\\') {
      output += "\\\\";
    } else {
      output += c;
    }
  }
  return output;
}

static void write_codspeed_benchmarks_to_json(
    const std::vector<CodspeedWalltimeBenchmark> &benchmarks) {
  std::ostringstream oss;

  std::string creator_name = "codspeed-cpp";
  std::string creator_version = CODSPEED_VERSION;
#ifdef _WIN32
  auto creator_pid = _getpid();
#else
  pid_t creator_pid = getpid();
#endif
  std::string instrument_type = "walltime";

  oss << "{\n";
  oss << "  \"creator\": {\n";
  oss << "    \"name\": \"" << creator_name << "\",\n";
  oss << "    \"version\": \"" << creator_version << "\",\n";
  oss << "    \"pid\": " << creator_pid << "\n";
  oss << "  },\n";
  oss << "  \"instrument\": {\n";
  oss << "    \"type\": \"" << instrument_type << "\"\n";
  oss << "  },\n";
  oss << "  \"benchmarks\": [\n";

  for (size_t i = 0; i < benchmarks.size(); ++i) {
    const auto &benchmark = benchmarks[i];
    const auto &stats = benchmark.stats;
    const auto &metadata = benchmark.metadata;

    oss << "    {\n";
    oss << "      \"name\": \"" << escape_backslashes(metadata.name) << "\",\n";
    oss << "      \"uri\": \"" << escape_backslashes(metadata.uri) << "\",\n";
    // TODO: Manage config fields from actual configuration
    oss << "      \"config\": {\n";
    oss << "        \"warmup_time_ns\": null,\n";
    oss << "        \"min_round_time_ns\": null,\n";
    oss << "        \"max_time_ns\": null,\n";
    oss << "        \"max_rounds\": null\n";
    oss << "      },\n";
    oss << "      \"stats\": {\n";
    oss << "        \"min_ns\": " << stats.min_ns << ",\n";
    oss << "        \"max_ns\": " << stats.max_ns << ",\n";
    oss << "        \"mean_ns\": " << stats.mean_ns << ",\n";
    oss << "        \"stdev_ns\": " << stats.stdev_ns << ",\n";
    oss << "        \"q1_ns\": " << stats.q1_ns << ",\n";
    oss << "        \"median_ns\": " << stats.median_ns << ",\n";
    oss << "        \"q3_ns\": " << stats.q3_ns << ",\n";
    oss << "        \"rounds\": " << stats.rounds << ",\n";
    oss << "        \"total_time\": " << stats.total_time << ",\n";
    oss << "        \"iqr_outlier_rounds\": " << stats.iqr_outlier_rounds
        << ",\n";
    oss << "        \"stdev_outlier_rounds\": " << stats.stdev_outlier_rounds
        << ",\n";
    oss << "        \"iter_per_round\": " << stats.iter_per_round << ",\n";
    oss << "        \"warmup_iters\": " << stats.warmup_iters << "\n";
    oss << "      }\n";
    oss << "    }";

    if (i < benchmarks.size() - 1) {
      oss << ",";
    }
    oss << "\n";
  }

  oss << "  ]\n";
  oss << "}";

  // Determine the directory path
  std::string profile_folder = safe_getenv("CODSPEED_PROFILE_FOLDER");
  std::string directory = profile_folder.empty() ? "." : profile_folder;

  // Create the results directory if it does not exist
  std::filesystem::path results_path = directory + "/results";
  if (!std::filesystem::exists(results_path)) {
    if (!std::filesystem::create_directories(results_path)) {
      std::cerr << "Failed to create directory: " << results_path << std::endl;
      return;
    }
  }

  // Create the file path
  std::ostringstream file_path;
  file_path << results_path.string() << "/" << creator_pid << ".json";

  // Write to file
  std::ofstream out_file(file_path.str());
  if (out_file.is_open()) {
    out_file << oss.str();
    out_file.close();
    std::cout << "JSON written to " << file_path.str() << std::endl;
  } else {
    std::cerr << "Unable to open file " << file_path.str() << std::endl;
  }
}

void generate_codspeed_walltime_report(
    const std::vector<RawWalltimeBenchmark> &raw_walltime_benchmarks) {
  std::vector<CodspeedWalltimeBenchmark> codspeed_walltime_benchmarks;

  for (const auto &raw_benchmark : raw_walltime_benchmarks) {
    assert(raw_benchmark.iters_per_round.size() ==
           raw_benchmark.times_per_round_ns.size());

    assert(raw_benchmark.iters_per_round.size() != 0);

    // Convert total round times to per-iteration times
    std::vector<double> per_iteration_times_ns;
    for (size_t i = 0; i < raw_benchmark.times_per_round_ns.size(); i++) {
      assert(raw_benchmark.iters_per_round[i] != 0);
      double per_iter_time_ns = raw_benchmark.times_per_round_ns[i] /
                                raw_benchmark.iters_per_round[i];
      per_iteration_times_ns.push_back(per_iter_time_ns);
    }

    // Sort for quantile computation
    std::vector<double> sorted_per_iter_times_ns = per_iteration_times_ns;
    std::sort(sorted_per_iter_times_ns.begin(), sorted_per_iter_times_ns.end());

    // Compute statistics from per-iteration times
    double mean_ns = std::accumulate(per_iteration_times_ns.begin(),
                                     per_iteration_times_ns.end(), 0.0) /
                     per_iteration_times_ns.size();

    double variance = 0.0;
    for (double time_ns : per_iteration_times_ns) {
      double diff = time_ns - mean_ns;
      variance += diff * diff;
    }
    double stdev_ns = std::sqrt(variance / per_iteration_times_ns.size());
    const double STDEV_OUTLIER_FACTOR = 3.0;
    size_t stdev_outlier_rounds = std::count_if(
        sorted_per_iter_times_ns.begin(), sorted_per_iter_times_ns.end(),
        [mean_ns, stdev_ns, STDEV_OUTLIER_FACTOR](double x) {
          return x < mean_ns - STDEV_OUTLIER_FACTOR * stdev_ns ||
                 x > mean_ns + STDEV_OUTLIER_FACTOR * stdev_ns;
        });

    double q1_ns = compute_quantile(sorted_per_iter_times_ns, 0.25);
    double median_ns = compute_quantile(sorted_per_iter_times_ns, 0.5);
    double q3_ns = compute_quantile(sorted_per_iter_times_ns, 0.75);

    double iqr_ns = q3_ns - q1_ns;
    const double IQR_OUTLIER_FACTOR = 1.5;
    size_t iqr_outlier_rounds = std::count_if(
        sorted_per_iter_times_ns.begin(), sorted_per_iter_times_ns.end(),
        [q1_ns, q3_ns, iqr_ns, IQR_OUTLIER_FACTOR](double x) {
          return x < q1_ns - IQR_OUTLIER_FACTOR * iqr_ns ||
                 x > q3_ns + IQR_OUTLIER_FACTOR * iqr_ns;
        });

    // Compute total time in seconds
    double total_time =
        std::accumulate(raw_benchmark.times_per_round_ns.begin(),
                        raw_benchmark.times_per_round_ns.end(), 0.0) /
        1e9;

    // TODO: CodSpeed format only supports one iter_per_round for all rounds,
    // for now take the average
    uint64_t avg_iters_per_round =
        std::accumulate(raw_benchmark.iters_per_round.begin(),
                        raw_benchmark.iters_per_round.end(), 0ULL) /
        raw_benchmark.iters_per_round.size();

    // Populate stats
    BenchmarkStats stats(*std::min_element(sorted_per_iter_times_ns.begin(),
                                           sorted_per_iter_times_ns.end()),
                         *std::max_element(sorted_per_iter_times_ns.begin(),
                                           sorted_per_iter_times_ns.end()),
                         mean_ns, stdev_ns, q1_ns, median_ns, q3_ns,
                         raw_benchmark.times_per_round_ns.size(), total_time,
                         iqr_outlier_rounds, stdev_outlier_rounds,
                         avg_iters_per_round,
                         0  // TODO: warmup_iters
    );
    CodspeedWalltimeBenchmark codspeed_benchmark;
    codspeed_benchmark.metadata = {raw_benchmark.name, raw_benchmark.uri};
    codspeed_benchmark.stats = stats;

    codspeed_walltime_benchmarks.push_back(codspeed_benchmark);
  }

  write_codspeed_benchmarks_to_json(codspeed_walltime_benchmarks);
}

}  // namespace codspeed
