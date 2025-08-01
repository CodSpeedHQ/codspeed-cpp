#ifndef MEASUREMENT_H
#define MEASUREMENT_H

#include <string>
#ifdef _WIN32
#include <process.h>
#define getpid _getpid
#define ALWAYS_INLINE __forceinline
#else
#include <unistd.h>
#define ALWAYS_INLINE __attribute__((always_inline)) inline
#endif

extern "C" {
#include "core.h"
}

extern InstrumentHooks* g_hooks;

void measurement_init();

inline std::string get_version() {
#ifdef CODSPEED_VERSION
  return {CODSPEED_VERSION};
#else
  return {""};
#endif
}

inline bool measurement_is_instrumented() {
  return instrument_hooks_is_instrumented(g_hooks);
}

inline void measurement_set_metadata() {
  std::string version = get_version();
  instrument_hooks_set_integration(g_hooks, "codspeed-cpp", version.c_str());
}

ALWAYS_INLINE void measurement_start() {
  instrument_hooks_start_benchmark_inline(g_hooks);
}

ALWAYS_INLINE void measurement_stop() {
  instrument_hooks_stop_benchmark_inline(g_hooks);
}

ALWAYS_INLINE void measurement_set_executed_benchmark(const std::string& name) {
  auto current_pid = getpid();
  instrument_hooks_executed_benchmark(g_hooks, current_pid, name.c_str());
}

#endif  // MEASUREMENT_H
