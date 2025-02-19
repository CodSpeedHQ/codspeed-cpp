#include "measurement.h"
#include "callgrind.h"
#include <iostream>

std::string get_version() {
#ifdef CODSPEED_VERSION
  return {CODSPEED_VERSION};
#else
  return {""};
#endif
}

bool measurement_is_instrumented() { return RUNNING_ON_VALGRIND; }

void measurement_set_metadata() {
  std::string metadata = "Metadata: codspeed-cpp " + get_version();
  CALLGRIND_DUMP_STATS_AT(metadata.c_str());
}

void measurement_start() {
  CALLGRIND_ZERO_STATS;
  CALLGRIND_START_INSTRUMENTATION;
}

void measurement_stop(const std::string &name) {
  CALLGRIND_STOP_INSTRUMENTATION;
  CALLGRIND_DUMP_STATS_AT(name.c_str());
};
