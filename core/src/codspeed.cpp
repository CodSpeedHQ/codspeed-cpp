#include "measurement.hpp"
#include <codspeed.h>
#include <iostream>
#include <string>
#include <vector>

std::string join(const std::vector<std::string> &elements,
                 const std::string &delimiter) {
  std::string result;
  for (size_t i = 0; i < elements.size(); ++i) {
    result += elements[i];
    if (i != elements.size() - 1) {
      result += delimiter;
    }
  }
  return result;
}

CodSpeed::CodSpeed() : is_instrumented(measurement_is_instrumented()) {
  if (!is_instrumented) {
    std::cerr
        << "NOTICE: codspeed is enabled, but no performance measurement will "
           "be made since it's running in an unknown environment."
        << std::endl;
  }
  measurement_set_metadata();
}

// Member function definitions
void CodSpeed::push_group(const std::string &group) {
  group_stack.push_back(group);
}

void CodSpeed::pop_group() {
  if (!group_stack.empty()) {
    group_stack.pop_back();
  }
}

void CodSpeed::start_benchmark(const std::string &name) {
  current_benchmark = name;
  measurement_start();
}

void CodSpeed::end_benchmark() {
  measurement_stop(current_benchmark);
  benchmarked.push_back(current_benchmark);
  std::string action_str = is_instrumented ? "Measured" : "Checked";
  std::string group_str =
      group_stack.empty() ? "" : " (group: " + join(group_stack, "/") + ")";
  std::cerr << action_str << ": " << current_benchmark << group_str
            << std::endl;
}
