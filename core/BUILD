load("@bazel_skylib//rules:common_settings.bzl", "string_flag")
load("@rules_cc//cc:defs.bzl", "cc_library")

CODSPEED_VERSION = "1.1.1"

# Define the codspeed library
cc_library(
    name = "codspeed",
    srcs = glob(["src/**/*.cpp"]),
    hdrs = glob(["include/**/*.h"] + ["include/**/*.hpp"]),
    includes = ["include"],
    defines = [
        "CODSPEED_VERSION=\\\"{}\\\"".format(CODSPEED_VERSION),
    ] + select({
        ":instrumentation_mode": ["CODSPEED_ENABLED", "CODSPEED_INSTRUMENTATION"],
        ":walltime_mode": ["CODSPEED_ENABLED", "CODSPEED_WALLTIME"],
        "//conditions:default": [],
    }),
    visibility = ["//visibility:public"],
)

# Codspeed mode
string_flag(
    name = "codspeed_mode",
    build_setting_default = "off",
    values = [
        "off",
        "instrumentation",
        "walltime",
    ],
)

config_setting(
    name = "instrumentation_mode",
    flag_values = {":codspeed_mode": "instrumentation"},
)

config_setting(
    name = "walltime_mode",
    flag_values = {":codspeed_mode": "walltime"},
)
