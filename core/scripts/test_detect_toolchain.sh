#!/usr/bin/env bash
# Manual tests for detect_toolchain.sh
# Run: bash core/scripts/test_detect_toolchain.sh
#
# Uses mock compiler scripts to simulate different --version outputs.

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
DETECT_SCRIPT="$SCRIPT_DIR/detect_toolchain.sh"
TMPDIR=$(mktemp -d)
trap 'rm -rf "$TMPDIR"' EXIT

PASS=0
FAIL=0

assert_contains() {
    local label="$1"
    local expected="$2"
    local actual="$3"
    if echo "$actual" | grep -qF "$expected"; then
        PASS=$((PASS + 1))
    else
        FAIL=$((FAIL + 1))
        echo "FAIL: $label"
        echo "  expected to contain: $expected"
        echo "  got: $actual"
    fi
}

# Create a mock compiler that outputs a given --version string
make_mock_compiler() {
    local name="$1"
    local version_output="$2"
    local path="$TMPDIR/$name"
    cat > "$path" << EOF
#!/usr/bin/env bash
echo "$version_output"
EOF
    chmod +x "$path"
    echo "$path"
}

# --- Test cases ---

echo "Running detect_toolchain.sh tests..."
echo

# Test: GCC
mock=$(make_mock_compiler "gcc-mock" "gcc (GCC) 14.3.0")
output=$(bash "$DETECT_SCRIPT" "$mock" "Release")
assert_contains "GCC compiler_id" 'CODSPEED_CXX_COMPILER_ID "GNU"' "$output"
assert_contains "GCC version" 'CODSPEED_CXX_COMPILER_VERSION "14.3.0"' "$output"
assert_contains "GCC full_version" 'CODSPEED_CXX_COMPILER_FULL_VERSION "gcc (GCC) 14.3.0"' "$output"
assert_contains "GCC build_type" 'CODSPEED_BUILD_TYPE "Release"' "$output"

# Test: g++
mock=$(make_mock_compiler "gpp-mock" "g++ (GCC) 13.2.1 20230801")
output=$(bash "$DETECT_SCRIPT" "$mock" "Debug")
assert_contains "g++ compiler_id" 'CODSPEED_CXX_COMPILER_ID "GNU"' "$output"
assert_contains "g++ version" 'CODSPEED_CXX_COMPILER_VERSION "13.2.1"' "$output"
assert_contains "g++ build_type" 'CODSPEED_BUILD_TYPE "Debug"' "$output"

# Test: Clang
mock=$(make_mock_compiler "clang-mock" "clang version 19.1.7 (https://github.com/llvm/llvm-project abc123)")
output=$(bash "$DETECT_SCRIPT" "$mock" "Release")
assert_contains "Clang compiler_id" 'CODSPEED_CXX_COMPILER_ID "Clang"' "$output"
assert_contains "Clang version" 'CODSPEED_CXX_COMPILER_VERSION "19.1.7"' "$output"

# Test: Apple Clang
mock=$(make_mock_compiler "apple-clang-mock" "Apple clang version 15.0.0 (clang-1500.0.40.1)")
output=$(bash "$DETECT_SCRIPT" "$mock" "Release")
assert_contains "Apple Clang compiler_id" 'CODSPEED_CXX_COMPILER_ID "Clang"' "$output"
assert_contains "Apple Clang version" 'CODSPEED_CXX_COMPILER_VERSION "15.0.0"' "$output"

# Test: Ubuntu GCC
mock=$(make_mock_compiler "ubuntu-gcc-mock" "gcc (Ubuntu 13.3.0-6ubuntu2~24.04) 13.3.0")
output=$(bash "$DETECT_SCRIPT" "$mock" "Release")
assert_contains "Ubuntu GCC compiler_id" 'CODSPEED_CXX_COMPILER_ID "GNU"' "$output"
assert_contains "Ubuntu GCC version" 'CODSPEED_CXX_COMPILER_VERSION "13.3.0"' "$output"

# Test: Unknown compiler
mock=$(make_mock_compiler "unknown-mock" "some-compiler v2.0")
output=$(bash "$DETECT_SCRIPT" "$mock" "Release")
assert_contains "Unknown compiler_id" 'CODSPEED_CXX_COMPILER_ID "unknown"' "$output"
assert_contains "Unknown version" 'CODSPEED_CXX_COMPILER_VERSION "unknown"' "$output"

# Test: Header guard present
assert_contains "Header guard" '#ifndef CODSPEED_TOOLCHAIN_INFO_H' "$output"
assert_contains "Header guard endif" '#endif' "$output"

# --- Summary ---

echo
TOTAL=$((PASS + FAIL))
if [ "$FAIL" -eq 0 ]; then
    echo "All $TOTAL tests passed."
else
    echo "$FAIL/$TOTAL tests FAILED."
    exit 1
fi
