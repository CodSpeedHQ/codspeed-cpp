#ifndef CODSPEED_ISOLATION_H
#define CODSPEED_ISOLATION_H

#include <functional>
#include <string>

namespace codspeed {

// Isolation is on unless this is `0` or `false`.
extern const char* const PROCESS_ISOLATION_ENV;
// Set on each child process: the URI of the only benchmark it runs.
extern const char* const ISOLATED_BENCHMARK_ENV;

// How the harness runs each benchmark. Every benchmark runs in a fresh copy of
// the benchmark executable by default, so the allocator state and threads that
// other benchmarks or setup code leave behind can't change its measurement.
class Isolation {
 public:
  enum class Kind {
    // All benchmarks share this process and its allocator state.
    SharedProcess,
    // Every benchmark runs in its own child process.
    ProcessPerBenchmark,
    // A child spawned by `ProcessPerBenchmark`, restricted to one URI.
    IsolatedBenchmark,
  };

  // Stores the original command line. Call it before the arguments get
  // parsed, because parsing removes the flags it consumed. The child process
  // is started with the same arguments.
  static void set_argv(int argc, const char* const* argv);

  // The isolation of this process, read once from the environment.
  static const Isolation& current();

  Kind kind() const { return kind_; }

  // The only benchmark this process may run, or null. It overrides CLI
  // filters.
  const std::string* only_benchmark() const;

  // Runs the benchmark `uri`. `bench` must contain the whole measurement,
  // from `start_benchmark` to `end_benchmark`.
  void run(const std::string& uri, const std::function<void()>& bench) const;

  // Runs this executable again with the same arguments, limited to `uri`, and
  // exits with the child's code if it fails.
  static void run_in_child_process(const std::string& uri);

 private:
  Isolation(Kind kind, std::string isolated_uri);
  static Isolation from_env();

  Kind kind_;
  std::string isolated_uri_;
};

}  // namespace codspeed

#endif  // CODSPEED_ISOLATION_H
