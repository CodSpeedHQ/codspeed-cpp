#!/usr/bin/env bash
# Detect C++ compiler toolchain information from `<compiler> --version` output.
# Used by Bazel genrule to generate toolchain_info.h.
#
# Usage: detect_toolchain.sh <compiler_path> <build_type>
# Output: Writes a C header to stdout with CODSPEED_CXX_* defines.

set -e

COMPILER="${1:?Usage: detect_toolchain.sh <compiler_path> <build_type>}"
BUILD_TYPE="${2:?Usage: detect_toolchain.sh <compiler_path> <build_type>}"

FULL_VERSION=$("$COMPILER" --version 2>/dev/null | head -n1) || FULL_VERSION="unknown"

COMPILER_ID="unknown"
COMPILER_VERSION="unknown"

if echo "$FULL_VERSION" | grep -qi "clang"; then
    COMPILER_ID="Clang"
    COMPILER_VERSION=$(echo "$FULL_VERSION" | grep -oP '\d+\.\d+\.\d+' | head -n1) || COMPILER_VERSION="unknown"
elif echo "$FULL_VERSION" | grep -qi "gcc\|g++\|GNU"; then
    COMPILER_ID="GNU"
    COMPILER_VERSION=$(echo "$FULL_VERSION" | grep -oP '\d+\.\d+\.\d+' | head -n1) || COMPILER_VERSION="unknown"
elif echo "$FULL_VERSION" | grep -qi "MSVC\|Microsoft"; then
    COMPILER_ID="MSVC"
    COMPILER_VERSION=$(echo "$FULL_VERSION" | grep -oP '\d+\.\d+\.\d+' | head -n1) || COMPILER_VERSION="unknown"
fi

cat << HEADER_EOF
// Auto-generated - do not edit
#ifndef CODSPEED_TOOLCHAIN_INFO_H
#define CODSPEED_TOOLCHAIN_INFO_H

#define CODSPEED_CXX_COMPILER_ID "$COMPILER_ID"
#define CODSPEED_CXX_COMPILER_VERSION "$COMPILER_VERSION"
#define CODSPEED_CXX_COMPILER_FULL_VERSION "$FULL_VERSION"
#define CODSPEED_BUILD_TYPE "$BUILD_TYPE"

#endif  // CODSPEED_TOOLCHAIN_INFO_H
HEADER_EOF
