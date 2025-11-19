# Codspeed C++

## Release

To create a release, run `scripts/release.sh <new_version>` from the main branch. This script will:
1. Automatically update the version in all relevant files:
    - [`core/CMakeLists.txt`](https://github.com/CodSpeedHQ/codspeed-cpp/blob/main/core/CMakeLists.txt#L3)
    - [`core/MODULE.bazel`](https://github.com/CodSpeedHQ/codspeed-cpp/blob/main/core/MODULE.bazel#L3)
    - [`google_benchmark/MODULE.bazel`](https://github.com/CodSpeedHQ/codspeed-cpp/blob/main/google_benchmark/MODULE.bazel)
2. Create a commit with the version changes
3. Generate the CHANGELOG
4. Tag and create the release on GitHub
