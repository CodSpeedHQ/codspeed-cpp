#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <random>
#include <string>

#include "codspeed.h"

namespace fs = std::filesystem;

// Builds a temporary tree, since the function checks that the file exists:
//   <tmp>/src/bench.cpp  the benchmark file
//   <tmp>/build/         the compile directory
//   <tmp>/other/deep/    a directory to run the benchmark from
class WorkspaceTest : public ::testing::Test {
 protected:
  void SetUp() override {
    root = CODSPEED_TEST_ROOT_DIR;
    if (root.empty()) {
      GTEST_SKIP() << "CODSPEED_ROOT_DIR is not set (git detection failed)";
    }
    previous_cwd = fs::current_path();

    // Canonical, so symlinks like /tmp -> /private/tmp on macOS don't change
    // the expected paths
    tmp = fs::canonical(fs::temp_directory_path()) /
          ("codspeed-workspace-test-" + std::to_string(std::random_device{}()));
    fs::create_directories(tmp / "src");
    fs::create_directories(tmp / "build");
    fs::create_directories(tmp / "other" / "deep");
    std::ofstream(tmp / "src" / "bench.cpp").close();

    file = tmp / "src" / "bench.cpp";
    expected = fs::relative(file, root);
  }

  void TearDown() override {
    if (!previous_cwd.empty()) {
      fs::current_path(previous_cwd);
    }
    if (!tmp.empty()) {
      std::error_code ec;
      fs::remove_all(tmp, ec);
    }
  }

  static fs::path relative(const std::string& path,
                           const std::string& compile_dir = "") {
    return codspeed::get_path_relative_to_workspace(path, compile_dir);
  }

  fs::path root;
  fs::path previous_cwd;
  fs::path tmp;
  fs::path file;
  fs::path expected;
};

TEST_F(WorkspaceTest, AbsolutePath) {
  EXPECT_EQ(relative(file.string()), expected);
  EXPECT_EQ(relative(file.string(), (tmp / "build").string()), expected);
}

// ccache with `base_dir`: __FILE__ is relative to the compile directory
TEST_F(WorkspaceTest, RelativeToCompileDir) {
  fs::current_path(tmp);
  EXPECT_EQ(relative("../src/bench.cpp", (tmp / "build").string()), expected);
}

// The benchmark runs from a directory that is not the compile directory's
// parent
TEST_F(WorkspaceTest, RelativeToCompileDirFromAnotherCwd) {
  fs::current_path(tmp / "other" / "deep");
  EXPECT_EQ(relative("../src/bench.cpp", (tmp / "build").string()), expected);
}

// The build directory can be deleted after the build
TEST_F(WorkspaceTest, RelativeToMissingCompileDir) {
  fs::current_path(tmp);
  EXPECT_EQ(relative("../src/bench.cpp", (tmp / "missing-build").string()),
            expected);
}

// `-fmacro-prefix-map=<root>/=` makes __FILE__ relative to the root, so the
// file does not exist in the compile directory
TEST_F(WorkspaceTest, FallsBackToCwdWhenMissingInCompileDir) {
  fs::current_path(tmp);
  EXPECT_EQ(relative("src/bench.cpp", (tmp / "build").string()), expected);
}

TEST_F(WorkspaceTest, RelativeToCwdWithoutCompileDir) {
  fs::current_path(tmp / "src");
  EXPECT_EQ(relative("bench.cpp"), expected);
}
