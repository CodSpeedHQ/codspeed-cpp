#ifndef MEASUREMENT_H
#define MEASUREMENT_H

#include <string>
#ifdef _WIN32
#include <process.h>
#else
#include <unistd.h>
#endif

#ifdef CODSPEED_INSTRUMENTATION
#include "callgrind.h"
#endif

extern "C" {
#include "core.h"
}

inline InstrumentHooks* get_hooks() {
  static InstrumentHooks* g_hooks = nullptr;
  if (!g_hooks) {
    g_hooks = instrument_hooks_init();
  }
  return g_hooks;
}

inline std::string get_version() {
#ifdef CODSPEED_VERSION
  return {CODSPEED_VERSION};
#else
  return {""};
#endif
}

#ifdef CODSPEED_INSTRUMENTATION
inline bool measurement_is_instrumented() {
  return instrument_hooks_is_instrumented(get_hooks());
}

inline void measurement_set_metadata() {
  std::string version = get_version();
  instrument_hooks_set_integration(get_hooks(), "codspeed-cpp",
                                   version.c_str());
}

__attribute__((always_inline)) inline void measurement_start() {
  // Keep the callgrind macros here, so that they are properly inlined.
  // Otherwise, we have an additional function call overhead to the
  // instrument-hooks library.
  CALLGRIND_ZERO_STATS;
  CALLGRIND_START_INSTRUMENTATION;

  instrument_hooks_start_benchmark(get_hooks());
}

__attribute__((always_inline)) inline void measurement_stop(
    const std::string& name) {
  CALLGRIND_STOP_INSTRUMENTATION;

  instrument_hooks_stop_benchmark(get_hooks());

#ifdef _WIN32
  auto current_pid = _getpid();
#else
  auto current_pid = getpid();
#endif
  instrument_hooks_executed_benchmark(get_hooks(), current_pid, name.c_str());
};
#else
// Stub implementations for non-instrumentation builds
inline bool measurement_is_instrumented() { return false; }
inline void measurement_set_metadata() {}
inline void measurement_start() {}
inline void measurement_stop(const std::string& name) { (void)name; }
#endif

#endif  // MEASUREMENT_H
