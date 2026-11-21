#include <benchmark/benchmark.h>
#include <unistd.h>

#include <condition_variable>
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

// Set to make `BM_Failing` exit with `FAILING_EXIT_CODE`.
static const char* const FAIL_ENV = "FIXTURE_FAIL";
static constexpr int FAILING_EXIT_CODE = 3;

// Started in `main`, before any benchmark, so its thread belongs to the
// process that runs the harness. A forked child would not have it.
class MainThreadWorker {
 public:
  MainThreadWorker() : thread_([this] { loop(); }) {}

  ~MainThreadWorker() {
    {
      std::lock_guard<std::mutex> lock(mutex_);
      done_ = true;
    }
    wakeup_.notify_one();
    thread_.join();
  }

  int submit(int value) {
    std::promise<int> result;
    {
      std::lock_guard<std::mutex> lock(mutex_);
      jobs_.push([&result, value] { result.set_value(value + 1); });
    }
    wakeup_.notify_one();
    return result.get_future().get();
  }

 private:
  void loop() {
    std::unique_lock<std::mutex> lock(mutex_);
    while (true) {
      wakeup_.wait(lock, [this] { return done_ || !jobs_.empty(); });
      if (jobs_.empty()) {
        return;
      }
      auto job = std::move(jobs_.front());
      jobs_.pop();
      lock.unlock();
      job();
      lock.lock();
    }
  }

  std::mutex mutex_;
  std::condition_variable wakeup_;
  std::queue<std::function<void()>> jobs_;
  bool done_ = false;
  std::thread thread_;
};

static MainThreadWorker* worker = nullptr;

// The test reads these lines to count the processes.
static void print_pid_once(const char* name) {
  static std::mutex print_mutex;
  std::lock_guard<std::mutex> lock(print_mutex);
  static std::string printed;
  if (printed.find(name) != std::string::npos) {
    return;
  }
  printed += name;
  std::printf("PID %s %d\n", name, static_cast<int>(getpid()));
  std::fflush(stdout);
}

static void BM_First(benchmark::State& state) {
  print_pid_once("first");
  for (auto _ : state) {
    benchmark::DoNotOptimize(1);
  }
}
BENCHMARK(BM_First);

static void BM_Second(benchmark::State& state) {
  print_pid_once("second");
  for (auto _ : state) {
    benchmark::DoNotOptimize(2);
  }
}
BENCHMARK(BM_Second);

static void BM_OnMainWorker(benchmark::State& state) {
  print_pid_once("worker");
  for (auto _ : state) {
    benchmark::DoNotOptimize(worker->submit(3));
  }
}
BENCHMARK(BM_OnMainWorker);

static void BM_Failing(benchmark::State& state) {
  print_pid_once("failing");
  for (auto _ : state) {
    std::exit(FAILING_EXIT_CODE);
  }
}

int main(int argc, char** argv) {
  MainThreadWorker main_worker;
  worker = &main_worker;

  if (std::getenv(FAIL_ENV) != nullptr) {
    benchmark::RegisterBenchmark("BM_Failing", BM_Failing);
  }

  benchmark::Initialize(&argc, argv);
  benchmark::RunSpecifiedBenchmarks();
  benchmark::Shutdown();
  return 0;
}
