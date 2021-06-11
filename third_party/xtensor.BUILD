load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "xtensor",
    hdrs = glob(["include/xtensor/*.hpp"]),
    defines = ["XTENSOR_USE_SIMD=1"],
    includes = ["include"],
    visibility = ["//visibility:public"],
    deps = [
        "@xsimd",
        "@xtl",
    ],
)
