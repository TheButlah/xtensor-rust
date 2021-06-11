workspace(name = "xtensor_rust")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "rules_rust",
    sha256 = "53d0a417fd945f5ca06eb9bb0c378c5f63338e93420e22e5fa6564dec9448d4b",
    strip_prefix = "rules_rust-087bcab8154f5c0d79980ad32cb6ffb8158de649",
    urls = [
        # Master branch as of 2021-06-11
        "https://github.com/bazelbuild/rules_rust/archive/087bcab8154f5c0d79980ad32cb6ffb8158de649.zip",
    ],
)

load("@rules_rust//rust:repositories.bzl", "rust_repositories")

rust_repositories(
    edition = "2018",
    version = "1.52.0",
)

# Dependencies for cxx

http_archive(
    name = "cxx.rs",
    # patches = ["//:build.patch"],
    repo_mapping = {"@third-party": "@cxx.rs_third-party"},
    sha256 = "ec47c18cffaedb56f31c9631e48950ac7f39012d0a4f9cc38536e74325a5dd99",
    strip_prefix = "cxx-1.0.49",
    url = "https://github.com/dtolnay/cxx/archive/refs/tags/1.0.49.tar.gz",
    # build_file = "//third_party:cxx.BUILD",
)

load("@cxx.rs//tools/bazel:vendor.bzl", cxx_vendor = "vendor")

cxx_vendor(
    name = "cxx.rs_third-party",
    lockfile = "@cxx.rs//third-party:Cargo.lock",
)

load("//third_party/cargo:crates.bzl", "raze_fetch_remote_crates")

raze_fetch_remote_crates()

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
