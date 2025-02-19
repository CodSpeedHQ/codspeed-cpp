#ifndef CODSPEED_H
#define CODSPEED_H

#include <string>
#include <vector>

class CodSpeed {
public:
  // Constructor
  CodSpeed();

  // Member functions
  void push_group(const std::string &group);
  void pop_group();
  void start_benchmark(const std::string &name);
  void end_benchmark();

private:
  std::vector<std::string> benchmarked;
  std::string current_benchmark;
  std::vector<std::string> group_stack;
  bool is_instrumented;
};

#endif // CODSPEED_H
