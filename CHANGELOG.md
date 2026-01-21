# Changelog


<sub>The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/), and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).</sub>



## [2.1.0] - 2026-01-21

### <!-- 0 -->🚀 Features
- Add memory benchmark example
- Run memory profiling in CI
- Define shared CODSPEED_MODE_DISPLAY
- Add support for memory mode

### <!-- 1 -->🐛 Bug Fixes
- Update release script, ensure correct versions are released
- Skip warmup in memory mode
- Exclude warmup measurements
- Dont start and stop valgrind twice

### <!-- 7 -->⚙️ Internals
- Use OIDC token
- Bump instrument-hooks
- Switch to simulation
- Bump instrument-hooks to support memory profiling


## [2.0.0] - 2025-11-21

### <!-- 0 -->🚀 Features
- Add support for automated BCR publication
- Update release process to add full tarball and bump versions
- Prepare repository for BCR release
- Accept rename instrumentation to simulation

### <!-- 7 -->⚙️ Internals
- Make release script work on macos
- Use c++17 to build core library


## [1.4.1] - 2025-11-07

### <!-- 0 -->🚀 Features
- Add unit tests for statistics computation
- Increase default codspeed walltime repetitions
- Add support for setting a custom benchmark name

### <!-- 1 -->🐛 Bug Fixes
- Fix walltime stats computation to handle total round time as an input
- Use seconds instead of nanoseconds for total_time
- Dont sample warmup with perf

### <!-- 7 -->⚙️ Internals
- Fix incorrect step name in bazel integration test


## [1.4.0] - 2025-10-22

### <!-- 0 -->🚀 Features
- Check multiple bazel versions
- Check multiple bazel versions on different os

### <!-- 1 -->🐛 Bug Fixes
- Initialize g_hooks before using it
- Ignore format errors
- Sync compiler flags with instrument-hooks; add strict mode
- Run cmake/bazel on many os
- Fetch instrument-hooks git repository
- Bazel build on v8+
- Bazel 9 build
- Push the commit in the release script

### <!-- 7 -->⚙️ Internals
- Enable strict warnings
- Bump instrument-hooks


## [1.3.0] - 2025-10-06

### <!-- 0 -->🚀 Features
- Add bench with `PauseTiming`
- Add multithread example
- Add support for benchmark markers
- Add walltime support to google benchmark
- Add instrument-hooks library
- Add root frame
- Add fibonacci benchmark
- Add sleep benches
- Add pre-commit-hooks

### <!-- 1 -->🐛 Bug Fixes
- Remove memory allocations, use fixed iteration count
- Ignore instrument-hooks warning
- Add mode param
- Dont optimize out benches

### <!-- 7 -->⚙️ Internals
- Add a changelog and a release script
- Bump instrument-hooks


## [1.2.0] - 2025-06-27

### <!-- 0 -->🚀 Features
- Add utils function to get env var in a safe way
- Allow user to manually set the root dir
- Allow installation as a system lib

### <!-- 1 -->🐛 Bug Fixes
- Allow project to build on windows
- Use proper type for pid on windows
- Remove compilation warning about unused variables
- Remove unused variables

### <!-- 7 -->⚙️ Internals
- Bump versions to 1.2.0
- Add windows minimal compilation test
- Prevent building tests if there are warnings
- Make build fail on any warning


## [1.1.1] - 2025-05-12

### <!-- 1 -->🐛 Bug Fixes
- Update source files pattern in codspeed cc_library target

### <!-- 7 -->⚙️ Internals
- Add --incompatible_disallow_empty_glob flag to Bazel config


## [1.1.0] - 2025-05-07

### <!-- 0 -->🚀 Features
- Add bazel support

### <!-- 2 -->🏗️ Refactor
- Add a codspeed namespace for codspeed core functions

### <!-- 7 -->⚙️ Internals
- Bump chore library version
- Add bazel integration tests
- Add clang-format config to the repo
- Change OFF to off to be inline with other CODSPEED_MODE values


## [1.0.1] - 2025-03-26

### <!-- 0 -->🚀 Features
- Expose codspeed mode as a cmake option
- Add a devShell for clang compilation
- Add support for fixtures

### <!-- 1 -->🐛 Bug Fixes
- Fix compilation error with -Wall in instrumentation mode
- Get pid instead of thread id

### <!-- 10 -->💼 Other
- Add debug info to example

### <!-- 7 -->⚙️ Internals
- Add CONTRIBUTING.md with release instructions


## [1.0.0] - 2025-03-13

### <!-- 0 -->🚀 Features
- Add tests for type double colon escape
- Add tests
- Escape double colons in type URI bench arguments
- Add namespace to URI
- Add walltime support
- Support arguments in URI
- Add file name to benchmark URI
- Only run benchmark once in instrumentation mode
- Make Codspeed class a singleton
- Allow measurement through codspeed core library
- Add very basic codspeed core library
- Add google_benchmark example
- Add flake for cpp development
- Initial commit

### <!-- 1 -->🐛 Bug Fixes
- Remarks from PR
- Generate proper uri

### <!-- 10 -->💼 Other
- Remove use of #pragma once
- Disable google benchmark lib tests

### <!-- 3 -->📚 Documentation
- Document anonymous namespaces as unsupported
- Add readme for monorepo and google benchmark compat layer

### <!-- 7 -->⚙️ Internals
- Add a license
- Remove partial picobench integration
- Rework benches
- Run walltime benchmarks in CI
- Build without codpseed in CI
- Add ci workflow
- Import google benchmark "fork"


[2.1.0]: https://github.com/CodSpeedHQ/runner/compare/v2.0.0..v2.1.0
[2.0.0]: https://github.com/CodSpeedHQ/runner/compare/v1.4.1..v2.0.0
[1.4.1]: https://github.com/CodSpeedHQ/runner/compare/v1.4.0..v1.4.1
[1.4.0]: https://github.com/CodSpeedHQ/runner/compare/v1.3.0..v1.4.0
[1.3.0]: https://github.com/CodSpeedHQ/runner/compare/v1.2.0..v1.3.0
[1.2.0]: https://github.com/CodSpeedHQ/runner/compare/v1.1.1..v1.2.0
[1.1.1]: https://github.com/CodSpeedHQ/runner/compare/v1.1.0..v1.1.1
[1.1.0]: https://github.com/CodSpeedHQ/runner/compare/v1.0.1..v1.1.0
[1.0.1]: https://github.com/CodSpeedHQ/runner/compare/v1.0.0..v1.0.1

<!-- generated by git-cliff -->
