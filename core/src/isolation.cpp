#include "isolation.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#include "utils.h"

#ifndef _WIN32
#include <spawn.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cerrno>
#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

extern char** environ;
#endif

namespace codspeed {

const char* const PROCESS_ISOLATION_ENV = "CODSPEED_PROCESS_ISOLATION";
const char* const ISOLATED_BENCHMARK_ENV = "CODSPEED_ISOLATED_BENCHMARK";

namespace {

// Exit code for a child that ended with neither an exit code nor a signal.
constexpr int GENERIC_FAILURE_EXIT_CODE = 1;
// Shells report a child killed by signal N as exit code 128 + N.
constexpr int SIGNAL_EXIT_CODE_OFFSET = 128;

#ifndef _WIN32
// Path of the running executable. `/proc/self/exe` keeps working when the
// benchmark changed its working directory or `argv[0]` is a bare name.
std::string current_executable() {
#if defined(__linux__)
  return "/proc/self/exe";
#elif defined(__APPLE__)
  uint32_t size = 0;
  _NSGetExecutablePath(nullptr, &size);
  std::vector<char> buffer(size);
  if (_NSGetExecutablePath(buffer.data(), &size) == 0) {
    return std::string(buffer.data());
  }
  return std::string();
#else
  return std::string();
#endif
}

// The environment of this process plus the isolated benchmark URI.
std::vector<std::string> child_environment(const std::string& uri) {
  std::vector<std::string> env;
  const std::string isolated_prefix = std::string(ISOLATED_BENCHMARK_ENV) + "=";
  for (char** entry = environ; entry != nullptr && *entry != nullptr; ++entry) {
    if (std::strncmp(*entry, isolated_prefix.c_str(), isolated_prefix.size()) ==
        0) {
      continue;
    }
    env.emplace_back(*entry);
  }
  env.push_back(isolated_prefix + uri);
  return env;
}

// Exit code of a failed child, in the shell convention.
int exit_code(int wait_status) {
  if (WIFSIGNALED(wait_status)) {
    return SIGNAL_EXIT_CODE_OFFSET + WTERMSIG(wait_status);
  }
  if (WIFEXITED(wait_status)) {
    return WEXITSTATUS(wait_status);
  }
  return GENERIC_FAILURE_EXIT_CODE;
}
#endif

std::vector<std::string>& original_argv() {
  static std::vector<std::string> argv;
  return argv;
}

}  // namespace

void Isolation::set_argv(int argc, const char* const* argv) {
  auto& stored = original_argv();
  stored.clear();
  for (int i = 0; i < argc; ++i) {
    stored.emplace_back(argv[i]);
  }
}

Isolation::Isolation(Kind kind, std::string isolated_uri)
    : kind_(kind), isolated_uri_(std::move(isolated_uri)) {}

const Isolation& Isolation::current() {
  static const Isolation instance = from_env();
  return instance;
}

Isolation Isolation::from_env() {
  const std::string uri = safe_getenv(ISOLATED_BENCHMARK_ENV);
  if (!uri.empty()) {
    return Isolation(Kind::IsolatedBenchmark, uri);
  }

  const std::string value = safe_getenv(PROCESS_ISOLATION_ENV);
  if (value.empty() || value == "1" || value == "true") {
    return Isolation(Kind::ProcessPerBenchmark, "");
  }
  if (value == "0" || value == "false") {
    return Isolation(Kind::SharedProcess, "");
  }

  std::cerr << PROCESS_ISOLATION_ENV << " must be `true` or `false`, got `"
            << value << "`" << std::endl;
  std::exit(GENERIC_FAILURE_EXIT_CODE);
}

const std::string* Isolation::only_benchmark() const {
  return kind_ == Kind::IsolatedBenchmark ? &isolated_uri_ : nullptr;
}

void Isolation::run(const std::string& uri,
                    const std::function<void()>& bench) const {
  switch (kind_) {
    case Kind::SharedProcess:
    case Kind::IsolatedBenchmark:
      bench();
      return;
    case Kind::ProcessPerBenchmark:
      run_in_child_process(uri);
      return;
  }
}

#ifdef _WIN32
void Isolation::run_in_child_process(const std::string& uri) {
  std::cerr << "process isolation is not supported on Windows, set "
            << PROCESS_ISOLATION_ENV << "=false (benchmark: " << uri << ")"
            << std::endl;
  std::exit(GENERIC_FAILURE_EXIT_CODE);
}
#else
void Isolation::run_in_child_process(const std::string& uri) {
  // Keep the parent's output before the child's.
  std::fflush(stdout);
  std::cout.flush();
  std::cerr.flush();

  const std::string exe = current_executable();
  const auto& args = original_argv();
  if (exe.empty() && args.empty()) {
    std::cerr << "failed to locate the benchmark executable for " << uri
              << std::endl;
    std::exit(GENERIC_FAILURE_EXIT_CODE);
  }

  // Fall back to argv[0] where the OS gives no executable path.
  const std::string& program = exe.empty() ? args[0] : exe;

  std::vector<std::string> argv_storage = args;
  if (argv_storage.empty()) {
    argv_storage.push_back(program);
  }
  std::vector<char*> argv;
  for (auto& arg : argv_storage) {
    argv.push_back(&arg[0]);
  }
  argv.push_back(nullptr);

  std::vector<std::string> env_storage = child_environment(uri);
  std::vector<char*> envp;
  for (auto& entry : env_storage) {
    envp.push_back(&entry[0]);
  }
  envp.push_back(nullptr);

  // Re-exec instead of fork(): fork only copies the calling thread, so threads
  // started before the harness (e.g. in main) would be missing in the child.
  pid_t pid = 0;
  const int spawn_error = posix_spawn(&pid, program.c_str(), nullptr, nullptr,
                                      argv.data(), envp.data());
  if (spawn_error != 0) {
    std::cerr << "failed to start the process for " << uri << ": "
              << std::strerror(spawn_error) << std::endl;
    std::exit(GENERIC_FAILURE_EXIT_CODE);
  }

  int wait_status = 0;
  while (waitpid(pid, &wait_status, 0) < 0) {
    if (errno == EINTR) {
      continue;
    }
    std::cerr << "failed to wait for the process of " << uri << ": "
              << std::strerror(errno) << std::endl;
    std::exit(GENERIC_FAILURE_EXIT_CODE);
  }

  if (WIFEXITED(wait_status) && WEXITSTATUS(wait_status) == 0) {
    return;
  }
  std::exit(exit_code(wait_status));
}
#endif

}  // namespace codspeed
