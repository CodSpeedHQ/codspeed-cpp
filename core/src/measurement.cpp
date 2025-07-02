#include "measurement.hpp"

InstrumentHooks* g_hooks = nullptr;

void measurement_init() {
  if (!g_hooks) {
    g_hooks = instrument_hooks_init();
  }
}
