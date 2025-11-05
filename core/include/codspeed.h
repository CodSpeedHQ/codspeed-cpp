#ifndef CODSPEED_H
#define CODSPEED_H

#include <cstdint>
#include <string>
#include <vector>

namespace codspeed {

class CodSpeed {
 public:
  // Public static method to access the single instance
  static CodSpeed *getInstance() {
    static CodSpeed instance;
    return &instance;
  }

  // Member functions
  void push_group(const std::string &group);
  void pop_group();
  void start_benchmark(const std::string &name);
  void end_benchmark();

 private:
  // Private constructor to prevent direct instantiation
  CodSpeed();
  std::vector<std::string> benchmarked;
  std::string current_benchmark;
  std::vector<std::string> group_stack;
  bool is_instrumented;
};

// Times are per iteration
struct RawWalltimeBenchmark {
  std::string name;
  std::string uri;
  // Number of iterations of each round
  std::vector<uint64_t> iters_per_round;
  // Time taken by each round, meaning all its iterations, in nanoseconds
  std::vector<double> times_per_round_ns;
};

void generate_codspeed_walltime_report(
    const std::vector<RawWalltimeBenchmark> &walltime_data_list);

std::string extract_lambda_namespace(const std::string &pretty_func);
std::string sanitize_bench_args(std::string &text);

// Gets path relative to workspace root, expected to be called with __FILE__ as
// an argument
std::string get_path_relative_to_workspace(const std::string &path);

}  // namespace codspeed

#endif  // CODSPEED_H
