"""
@generated
cargo-raze generated Bazel file.

DO NOT EDIT! Replaced on runs of cargo-raze
"""

load("@bazel_tools//tools/build_defs/repo:git.bzl", "new_git_repository")  # buildifier: disable=load
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")  # buildifier: disable=load
load("@bazel_tools//tools/build_defs/repo:utils.bzl", "maybe")  # buildifier: disable=load

def raze_fetch_remote_crates():
    """This function defines a collection of repos and should be called in a WORKSPACE file"""
    maybe(
        http_archive,
        name = "raze__autocfg__1_0_1",
        url = "https://crates.io/api/v1/crates/autocfg/1.0.1/download",
        type = "tar.gz",
        sha256 = "cdb031dd78e28731d87d56cc8ffef4a8f36ca26c38fe2de700543e627f8a464a",
        strip_prefix = "autocfg-1.0.1",
        build_file = Label("//third_party/cargo/remote:BUILD.autocfg-1.0.1.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__cc__1_0_68",
        url = "https://crates.io/api/v1/crates/cc/1.0.68/download",
        type = "tar.gz",
        sha256 = "4a72c244c1ff497a746a7e1fb3d14bd08420ecda70c8f25c7112f2781652d787",
        strip_prefix = "cc-1.0.68",
        build_file = Label("//third_party/cargo/remote:BUILD.cc-1.0.68.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__cxx__1_0_49",
        url = "https://crates.io/api/v1/crates/cxx/1.0.49/download",
        type = "tar.gz",
        sha256 = "65369ae12626651c9d8c5ca8b44ffdb9fe7ba710508074d2997315d2fe4f4276",
        strip_prefix = "cxx-1.0.49",
        build_file = Label("//third_party/cargo/remote:BUILD.cxx-1.0.49.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__cxxbridge_flags__1_0_49",
        url = "https://crates.io/api/v1/crates/cxxbridge-flags/1.0.49/download",
        type = "tar.gz",
        sha256 = "f9198d12d90c4a743fa06d45901ea419b81f250ad8f97635763cfaa791429ba1",
        strip_prefix = "cxxbridge-flags-1.0.49",
        build_file = Label("//third_party/cargo/remote:BUILD.cxxbridge-flags-1.0.49.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__cxxbridge_macro__1_0_49",
        url = "https://crates.io/api/v1/crates/cxxbridge-macro/1.0.49/download",
        type = "tar.gz",
        sha256 = "63138f3cc0d24ef4c5a71e0def78e5375bd52d17a58cefc645b091c361dc7c3e",
        strip_prefix = "cxxbridge-macro-1.0.49",
        build_file = Label("//third_party/cargo/remote:BUILD.cxxbridge-macro-1.0.49.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__link_cplusplus__1_0_5",
        url = "https://crates.io/api/v1/crates/link-cplusplus/1.0.5/download",
        type = "tar.gz",
        sha256 = "8f1becd27d473556dc610b8afa1636ef90747b574a84553bc11e82371d5ef2d1",
        strip_prefix = "link-cplusplus-1.0.5",
        build_file = Label("//third_party/cargo/remote:BUILD.link-cplusplus-1.0.5.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__matrixmultiply__0_3_1",
        url = "https://crates.io/api/v1/crates/matrixmultiply/0.3.1/download",
        type = "tar.gz",
        sha256 = "5a8a15b776d9dfaecd44b03c5828c2199cddff5247215858aac14624f8d6b741",
        strip_prefix = "matrixmultiply-0.3.1",
        build_file = Label("//third_party/cargo/remote:BUILD.matrixmultiply-0.3.1.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__ndarray__0_15_3",
        url = "https://crates.io/api/v1/crates/ndarray/0.15.3/download",
        type = "tar.gz",
        sha256 = "08e854964160a323e65baa19a0b1a027f76d590faba01f05c0cbc3187221a8c9",
        strip_prefix = "ndarray-0.15.3",
        build_file = Label("//third_party/cargo/remote:BUILD.ndarray-0.15.3.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__num_complex__0_4_0",
        url = "https://crates.io/api/v1/crates/num-complex/0.4.0/download",
        type = "tar.gz",
        sha256 = "26873667bbbb7c5182d4a37c1add32cdf09f841af72da53318fdb81543c15085",
        strip_prefix = "num-complex-0.4.0",
        build_file = Label("//third_party/cargo/remote:BUILD.num-complex-0.4.0.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__num_integer__0_1_44",
        url = "https://crates.io/api/v1/crates/num-integer/0.1.44/download",
        type = "tar.gz",
        sha256 = "d2cc698a63b549a70bc047073d2949cce27cd1c7b0a4a862d08a8031bc2801db",
        strip_prefix = "num-integer-0.1.44",
        build_file = Label("//third_party/cargo/remote:BUILD.num-integer-0.1.44.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__num_traits__0_2_14",
        url = "https://crates.io/api/v1/crates/num-traits/0.2.14/download",
        type = "tar.gz",
        sha256 = "9a64b1ec5cda2586e284722486d802acf1f7dbdc623e2bfc57e65ca1cd099290",
        strip_prefix = "num-traits-0.2.14",
        build_file = Label("//third_party/cargo/remote:BUILD.num-traits-0.2.14.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__paste__1_0_5",
        url = "https://crates.io/api/v1/crates/paste/1.0.5/download",
        type = "tar.gz",
        sha256 = "acbf547ad0c65e31259204bd90935776d1c693cec2f4ff7abb7a1bbbd40dfe58",
        strip_prefix = "paste-1.0.5",
        build_file = Label("//third_party/cargo/remote:BUILD.paste-1.0.5.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__proc_macro2__1_0_27",
        url = "https://crates.io/api/v1/crates/proc-macro2/1.0.27/download",
        type = "tar.gz",
        sha256 = "f0d8caf72986c1a598726adc988bb5984792ef84f5ee5aa50209145ee8077038",
        strip_prefix = "proc-macro2-1.0.27",
        build_file = Label("//third_party/cargo/remote:BUILD.proc-macro2-1.0.27.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__quote__1_0_9",
        url = "https://crates.io/api/v1/crates/quote/1.0.9/download",
        type = "tar.gz",
        sha256 = "c3d0b9745dc2debf507c8422de05d7226cc1f0644216dfdfead988f9b1ab32a7",
        strip_prefix = "quote-1.0.9",
        build_file = Label("//third_party/cargo/remote:BUILD.quote-1.0.9.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__rawpointer__0_2_1",
        url = "https://crates.io/api/v1/crates/rawpointer/0.2.1/download",
        type = "tar.gz",
        sha256 = "60a357793950651c4ed0f3f52338f53b2f809f32d83a07f72909fa13e4c6c1e3",
        strip_prefix = "rawpointer-0.2.1",
        build_file = Label("//third_party/cargo/remote:BUILD.rawpointer-0.2.1.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__syn__1_0_73",
        url = "https://crates.io/api/v1/crates/syn/1.0.73/download",
        type = "tar.gz",
        sha256 = "f71489ff30030d2ae598524f61326b902466f72a0fb1a8564c001cc63425bcc7",
        strip_prefix = "syn-1.0.73",
        build_file = Label("//third_party/cargo/remote:BUILD.syn-1.0.73.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__unicode_xid__0_2_2",
        url = "https://crates.io/api/v1/crates/unicode-xid/0.2.2/download",
        type = "tar.gz",
        sha256 = "8ccb82d61f80a663efe1f787a51b16b5a51e3314d6ac365b08639f52387b33f3",
        strip_prefix = "unicode-xid-0.2.2",
        build_file = Label("//third_party/cargo/remote:BUILD.unicode-xid-0.2.2.bazel"),
    )
