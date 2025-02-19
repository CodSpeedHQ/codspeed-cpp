#ifndef MEASUREMENT_H
#define MEASUREMENT_H

#include <string>

bool measurement_is_instrumented();
void measurement_set_metadata();
void measurement_start();
void measurement_stop(const std::string &name);

#endif // MEASUREMENT_H
