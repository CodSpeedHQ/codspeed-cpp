#include "benchmark_api_internal.h"

#include <cinttypes>

#include "codspeed.h"
#include "string_util.h"
#include "thread_timer.h"

namespace benchmark {
namespace internal {

BenchmarkInstance::BenchmarkInstance(Benchmark* benchmark, int family_idx,
                                     int per_family_instance_idx,
                                     const std::vector<int64_t>& args,
                                     int thread_count)
    : benchmark_(*benchmark),
      family_index_(family_idx),
      per_family_instance_index_(per_family_instance_idx),
      aggregation_report_mode_(benchmark_.aggregation_report_mode_),
      args_(args),
      time_unit_(benchmark_.GetTimeUnit()),
      measure_process_cpu_time_(benchmark_.measure_process_cpu_time_),
      use_real_time_(benchmark_.use_real_time_),
      use_manual_time_(benchmark_.use_manual_time_),
      complexity_(benchmark_.complexity_),
      complexity_lambda_(benchmark_.complexity_lambda_),
      statistics_(benchmark_.statistics_),
      repetitions_(benchmark_.repetitions_),
      min_time_(benchmark_.min_time_),
      min_warmup_time_(benchmark_.min_warmup_time_),
      iterations_(benchmark_.iterations_),
      threads_(thread_count) {
  name_.function_name = benchmark_.name_;

  size_t arg_i = 0;
  for (const auto& arg : args) {
    if (!name_.args.empty()) {
      name_.args += '/';
    }

    if (arg_i < benchmark->arg_names_.size()) {
      const auto& arg_name = benchmark_.arg_names_[arg_i];
      if (!arg_name.empty()) {
        name_.args += StrFormat("%s:", arg_name.c_str());
      }
    }

    name_.args += StrFormat("%" PRId64, arg);
    ++arg_i;
  }

  if (!IsZero(benchmark->min_time_)) {
    name_.min_time = StrFormat("min_time:%0.3f", benchmark_.min_time_);
  }

  if (!IsZero(benchmark->min_warmup_time_)) {
    name_.min_warmup_time =
        StrFormat("min_warmup_time:%0.3f", benchmark_.min_warmup_time_);
  }

  if (benchmark_.iterations_ != 0) {
    name_.iterations = StrFormat(
        "iterations:%lu", static_cast<unsigned long>(benchmark_.iterations_));
  }

  if (benchmark_.repetitions_ != 0) {
    name_.repetitions = StrFormat("repeats:%d", benchmark_.repetitions_);
  }

  if (benchmark_.measure_process_cpu_time_) {
    name_.time_type = "process_time";
  }

  if (benchmark_.use_manual_time_) {
    if (!name_.time_type.empty()) {
      name_.time_type += '/';
    }
    name_.time_type += "manual_time";
  } else if (benchmark_.use_real_time_) {
    if (!name_.time_type.empty()) {
      name_.time_type += '/';
    }
    name_.time_type += "real_time";
  }

  if (!benchmark_.thread_counts_.empty()) {
    name_.threads = StrFormat("threads:%d", threads_);
  }

  setup_ = benchmark_.setup_;
  teardown_ = benchmark_.teardown_;
}

#ifdef CODSPEED_INSTRUMENTATION
State BenchmarkInstance::RunInstrumented(
    codspeed::CodSpeed* codspeed, internal::ThreadTimer* timer,
    internal::ThreadManager* manager,
    internal::PerfCountersMeasurement* perf_counters_measurement,
    ProfilerManager* profiler_manager) const {
  // Do one repetition to avoid flakiness due to inconcistencies in CPU cache
  // from execution order

  internal::ThreadTimer warmup_timer = internal::ThreadTimer::Create();
  State warmup_state(name_.function_name, 1, args_, 0, 1, &warmup_timer,
                     manager, perf_counters_measurement, profiler_manager,
                     NULL);
  benchmark_.Run(warmup_state);

  State st(name().str(), 1, args_, 0, 1, timer, manager,
           perf_counters_measurement, profiler_manager, codspeed);
  ;
  benchmark_.Run(st);
  return st;
}
#endif

State BenchmarkInstance::__codspeed_root_frame__Run(
    IterationCount iters, int thread_id, internal::ThreadTimer* timer,
    internal::ThreadManager* manager,
    internal::PerfCountersMeasurement* perf_counters_measurement,
    ProfilerManager* profiler_manager) const {
#ifdef CODSPEED_WALLTIME
  State st(name_.function_name, iters, args_, thread_id, threads_, timer,
           manager, perf_counters_measurement, profiler_manager, codspeed::CodSpeed::getInstance());
#else
  State st(name_.function_name, iters, args_, thread_id, threads_, timer,
           manager, perf_counters_measurement, profiler_manager);
#endif
  benchmark_.Run(st);
  return st;
}

void BenchmarkInstance::Setup() const {
  if (setup_ != nullptr) {
    State st(name_.function_name, /*iters*/ 1, args_, /*thread_id*/ 0, threads_,
             nullptr, nullptr, nullptr, nullptr);
    setup_(st);
  }
}

void BenchmarkInstance::Teardown() const {
  if (teardown_ != nullptr) {
    State st(name_.function_name, /*iters*/ 1, args_, /*thread_id*/ 0, threads_,
             nullptr, nullptr, nullptr, nullptr);
    teardown_(st);
  }
}
}  // namespace internal
}  // namespace benchmark
