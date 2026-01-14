load("@bazel_skylib//rules:common_settings.bzl", "string_flag")
load("@rules_cc//cc:defs.bzl", "cc_library")

CODSPEED_VERSION = module_version()

config_setting(
    name = "windows",
    constraint_values = ["@platforms//os:windows"],
)

# Strict warnings mode
string_flag(
    name = "strict_warnings",
    build_setting_default = "off",
    values = [
        "off",
        "on",
    ],
)

config_setting(
    name = "strict_warnings_enabled",
    flag_values = {":strict_warnings": "on"},
)

config_setting(
    name = "windows_strict_warnings",
    constraint_values = ["@platforms//os:windows"],
    flag_values = {":strict_warnings": "on"},
)

# IMPORTANT: Keep in sync with instrument-hooks/ci.yml (COMMON_CFLAGS)
instrument_hooks_cflags = [
    "-Wno-format",
    "-Wno-format-security",
    "-Wno-unused-but-set-variable",
    "-Wno-unused-const-variable",
    "-Wno-type-limits",
    "-Wno-uninitialized",
    # When cross-compiling:
    "-Wno-constant-conversion",
    "-Wno-incompatible-pointer-types",
    "-Wno-unused-function",
]

instrument_hooks_cflags_windows = [
    "/wd4101",  # unreferenced local variable (equivalent to -Wno-unused-variable)
    "/wd4189",  # local variable is initialized but not referenced (equivalent to -Wno-unused-but-set-variable)
    "/wd4100",  # unreferenced formal parameter (equivalent to -Wno-unused-parameter)
    "/wd4245",  # signed/unsigned mismatch
    "/wd4132",  # const object should be initialized
    "/wd4146",  # unary minus operator applied to unsigned type
]

# Instrument-hooks library with warning suppressions
cc_library(
    name = "instrument_hooks",
    srcs = ["instrument-hooks/dist/core.c"],
    hdrs = glob(["instrument-hooks/includes/*.h"]),
    includes = ["instrument-hooks/includes"],
    copts = select({
        ":windows_strict_warnings": ["/W4","/WX"] + instrument_hooks_cflags_windows,
        ":windows": instrument_hooks_cflags_windows,
        ":strict_warnings_enabled": ["-Wall", "-Werror"] + instrument_hooks_cflags,
        "//conditions:default": instrument_hooks_cflags,
    }),
    visibility = ["//visibility:public"],
)


# Define the codspeed library
cc_library(
    name = "codspeed",
    srcs = glob(["src/**/*.cpp"] + ["src/**/*.h"]),
    hdrs = glob(["include/**/*.h"] + ["include/**/*.hpp"]),
    includes = ["include"],
    copts = select({
        ":windows": ["/std:c++17"],
        "//conditions:default": ["-std=c++17"],
    }),
    defines = [
        "CODSPEED_VERSION=\\\"{}\\\"".format(CODSPEED_VERSION),
    ] + select({
        ":instrumentation_mode": ["CODSPEED_ENABLED", "CODSPEED_ANALYSIS"],
        ":simulation_mode": ["CODSPEED_ENABLED", "CODSPEED_ANALYSIS"],
        ":memory_mode": ["CODSPEED_ENABLED", "CODSPEED_ANALYSIS"],
        ":walltime_mode": ["CODSPEED_ENABLED", "CODSPEED_WALLTIME"],
        "//conditions:default": [],
    }),
    deps = [":instrument_hooks"],
    visibility = ["//visibility:public"],
)

# Codspeed mode
string_flag(
    name = "codspeed_mode",
    build_setting_default = "off",
    values = [
        "off",
        "instrumentation",
        "simulation",
        "memory",
        "walltime",
    ],
    visibility = ["//visibility:public"],
)

config_setting(
    name = "instrumentation_mode",
    flag_values = {":codspeed_mode": "instrumentation"},
)

config_setting(
    name = "simulation_mode",
    flag_values = {":codspeed_mode": "simulation"},
)

config_setting(
    name = "memory_mode",
    flag_values = {":codspeed_mode": "memory"},
)

config_setting(
    name = "walltime_mode",
    flag_values = {":codspeed_mode": "walltime"},
)
