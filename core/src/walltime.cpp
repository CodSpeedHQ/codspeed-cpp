#include "codspeed.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string>
#ifdef _WIN32
#include <process.h>
#else
#include <unistd.h>
#endif
#include <vector>

const double IQR_OUTLIER_FACTOR = 1.5;
const double STDEV_OUTLIER_FACTOR = 3.0;

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
  long iter_per_round;
  uint64_t warmup_iters;
};

struct BenchmarkMetadata {
  std::string name;
  std::string uri;
};

struct CodspeedWalltimeBenchmark {
  BenchmarkMetadata metadata;
  BenchmarkStats stats;
};

double compute_quantile(const std::vector<double> &data, double quantile) {
  size_t n = data.size();
  if (n == 0)
    return 0.0;

  double pos = quantile * (n - 1);
  size_t k = static_cast<size_t>(pos);
  double d = pos - k;

  if (k + 1 < n) {
    return data[k] + d * (data[k + 1] - data[k]);
  }
  return data[k];
}

void compute_iqr_and_outliers(const std::vector<double> &times_ns, double mean,
                              double median, double stdev, double &q1,
                              double &q3, double &iqr,
                              size_t &iqr_outlier_rounds,
                              size_t &stdev_outlier_rounds) {
  std::vector<double> sorted_times = times_ns;
  std::sort(sorted_times.begin(), sorted_times.end());

  q1 = compute_quantile(sorted_times, 0.25);
  q3 = compute_quantile(sorted_times, 0.75);

  iqr = q3 - q1;

  const double IQR_OUTLIER_FACTOR = 1.5;
  const double STDEV_OUTLIER_FACTOR = 3.0;

  iqr_outlier_rounds =
      std::count_if(sorted_times.begin(), sorted_times.end(),
                    [q1, q3, iqr, IQR_OUTLIER_FACTOR](double x) {
                      return x < q1 - IQR_OUTLIER_FACTOR * iqr ||
                             x > q3 + IQR_OUTLIER_FACTOR * iqr;
                    });

  stdev_outlier_rounds =
      std::count_if(sorted_times.begin(), sorted_times.end(),
                    [mean, stdev, STDEV_OUTLIER_FACTOR](double x) {
                      return x < mean - STDEV_OUTLIER_FACTOR * stdev ||
                             x > mean + STDEV_OUTLIER_FACTOR * stdev;
                    });
}

std::string escapeBackslashes(const std::string &input) {
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

void write_codspeed_benchmarks_to_json(
    const std::vector<CodspeedWalltimeBenchmark> &benchmarks) {
  std::ostringstream oss;

  std::string creator_name = "codspeed-cpp";
  std::string creator_version = CODSPEED_VERSION;
#ifdef _WIN32
  pid_t creator_pid = _getpid();
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
    oss << "      \"name\": \"" << escapeBackslashes(metadata.name) << "\",\n";
    oss << "      \"uri\": \"" << escapeBackslashes(metadata.uri) << "\",\n";
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
  const char *profile_folder = std::getenv("CODSPEED_PROFILE_FOLDER");
  std::string directory = profile_folder ? profile_folder : ".";

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
    CodspeedWalltimeBenchmark codspeed_benchmark;
    codspeed_benchmark.metadata = {raw_benchmark.name, raw_benchmark.uri};

    double total_time =
        std::accumulate(raw_benchmark.round_times_ns.begin(),
                        raw_benchmark.round_times_ns.end(), 0.0);

    double mean = raw_benchmark.mean_ns;
    double median = raw_benchmark.median_ns;
    double stdev = raw_benchmark.stdev_ns;
    double q1, q3, iqr;
    size_t iqr_outlier_rounds, stdev_outlier_rounds;
    compute_iqr_and_outliers(raw_benchmark.round_times_ns, mean, median, stdev,
                             q1, q3, iqr, iqr_outlier_rounds,
                             stdev_outlier_rounds);

    // Populate stats
    codspeed_benchmark.stats = {
        *std::min_element(raw_benchmark.round_times_ns.begin(),
                          raw_benchmark.round_times_ns.end()),
        *std::max_element(raw_benchmark.round_times_ns.begin(),
                          raw_benchmark.round_times_ns.end()),
        mean,
        stdev,
        q1,
        median,
        q3,
        raw_benchmark.round_times_ns.size(),
        total_time,
        iqr_outlier_rounds,
        stdev_outlier_rounds,
        raw_benchmark.iter_per_round,
        0 // TODO: warmup_iters
    };

    codspeed_walltime_benchmarks.push_back(codspeed_benchmark);
  }

  write_codspeed_benchmarks_to_json(codspeed_walltime_benchmarks);
}
