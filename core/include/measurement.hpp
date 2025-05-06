#ifndef MEASUREMENT_H
#define MEASUREMENT_H

#include <string>

#include "callgrind.h"

inline std::string get_version() {
#ifdef CODSPEED_VERSION
  return {CODSPEED_VERSION};
#else
  return {""};
#endif
}

inline bool measurement_is_instrumented() { return RUNNING_ON_VALGRIND; }

inline void measurement_set_metadata() {
  std::string metadata = "Metadata: codspeed-cpp " + get_version();
  CALLGRIND_DUMP_STATS_AT(metadata.c_str());
}

__attribute__((always_inline)) inline void measurement_start() {
  CALLGRIND_ZERO_STATS;
  CALLGRIND_START_INSTRUMENTATION;
}

__attribute__((always_inline)) inline void measurement_stop(
    const std::string &name) {
  CALLGRIND_STOP_INSTRUMENTATION;
  CALLGRIND_DUMP_STATS_AT(name.c_str());
};

#endif  // MEASUREMENT_H
