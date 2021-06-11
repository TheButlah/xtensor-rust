workspace(name = "xtensor_rust")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# Dependencies for cxx

# http_archive(
#     name = "rules_rust",
#     sha256 = "18c0a02a007cd26c3f5b4d21dc26a80af776ef755460028a796bc61c649fdf3f",
#     strip_prefix = "rules_rust-467a301fd665db344803c1d8a2401ec2bf8c74ce",
#     urls = [
#         # Master branch as of 2021-04-23
#         "https://github.com/bazelbuild/rules_rust/archive/467a301fd665db344803c1d8a2401ec2bf8c74ce.tar.gz",
#     ],
# )

# load("@rules_rust//rust:repositories.bzl", "rust_repositories")

# rust_repositories()

http_archive(
    name = "cxx",
    build_file = "//third_party:cxx.BUILD",
    sha256 = "ec47c18cffaedb56f31c9631e48950ac7f39012d0a4f9cc38536e74325a5dd99",
    strip_prefix = "cxx-1.0.49",
    url = "https://github.com/dtolnay/cxx/archive/refs/tags/1.0.49.tar.gz",
)

# Dependencies for xtensor

http_archive(
    name = "xtl",
    build_file = "//third_party:xtl.BUILD",
    sha256 = "d9ce8c05b3cb4ef26133cbd4a0818598e28c14f4bf65bb3b8c1dfa14c4f9e349",
    strip_prefix = "xtl-0.6.13",
    url = "https://github.com/xtensor-stack/xtl/archive/0.6.13.zip",
)

http_archive(
    name = "xsimd",
    build_file = "//third_party:xsimd.BUILD",
    sha256 = "318971a98416d289eaf280db6f20d0fb85fde4309766b4149ba72292315f7a9f",
    strip_prefix = "xsimd-7.4.6",
    url = "https://github.com/xtensor-stack/xsimd/archive/7.4.6.zip",
)

http_archive(
    name = "xtensor",
    build_file = "//third_party:xtensor.BUILD",
    sha256 = "63fec2d0b81dac5d76e5cf7dd0260bc8d57d768f12fc37da7513b4850aa2109d",
    strip_prefix = "xtensor-0.21.4",
    url = "https://github.com/xtensor-stack/xtensor/archive/0.21.4.zip",
)

# Dependencies for testing

http_archive(
    name = "catch2",
    build_file = "//third_party:catch2.BUILD",
    sha256 = "c5a0a7510379c6f37f70b329986a335a7b8489d67ac417ce8f4262d0cae4cc5d",
    strip_prefix = "Catch2-2.11.3",
    url = "https://github.com/catchorg/Catch2/archive/v2.13.6.zip",
)
