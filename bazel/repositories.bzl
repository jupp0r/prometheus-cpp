load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:utils.bzl", "maybe")

def prometheus_cpp_repositories():
    maybe(
        http_archive,
        name = "civetweb",
        strip_prefix = "civetweb-1.12",
        sha256 = "8cab1e2ad8fb3e2e81fed0b2321a5afbd7269a644c44ed4c3607e0a212c6d9e1",
        urls = [
            "https://github.com/civetweb/civetweb/archive/v1.12.tar.gz",
        ],
        build_file = "@com_github_jupp0r_prometheus_cpp//bazel:civetweb.BUILD",
    )

    maybe(
        http_archive,
        name = "com_google_absl",
        urls = [
            "https://github.com/abseil/abseil-cpp/archive/a1d6689907864974118e592ef2ac7d716c576aad.tar.gz",
        ],
        strip_prefix = "abseil-cpp-a1d6689907864974118e592ef2ac7d716c576aad",
        sha256 = "bc6bd9adc8a8e514c019a8369195b3d5c80e7edc22602b9252babdb624439dd1",
    )

    maybe(
        http_archive,
        name = "com_google_googletest",
        sha256 = "9dc9157a9a1551ec7a7e43daea9a694a0bb5fb8bec81235d8a1e6ef64c716dcb",
        strip_prefix = "googletest-release-1.10.0",
        urls = [
            "https://github.com/google/googletest/archive/release-1.10.0.tar.gz",
        ],
    )

    maybe(
        http_archive,
        name = "com_google_tcmalloc",
        urls = [
            "https://github.com/google/tcmalloc/archive/82969a21d3f99096e1c79b5e59853bdb4e358474.tar.gz",
        ],
        strip_prefix = "tcmalloc-82969a21d3f99096e1c79b5e59853bdb4e358474",
        sha256 = "f2ef9e1ca43bd6a496f67e3100d728648cf8ba2336bbd5c255390d9dc2eb7692",
    )

    maybe(
        http_archive,
        name = "com_github_curl",
        sha256 = "01ae0c123dee45b01bbaef94c0bc00ed2aec89cb2ee0fd598e0d302a6b5e0a98",
        strip_prefix = "curl-7.69.1",
        urls = [
            "https://github.com/curl/curl/releases/download/curl-7_69_1/curl-7.69.1.tar.gz",
            "https://curl.haxx.se/download/curl-7.69.1.tar.gz",
        ],
        build_file = "@com_github_jupp0r_prometheus_cpp//bazel:curl.BUILD",
    )

    maybe(
        http_archive,
        name = "com_github_google_benchmark",
        sha256 = "3c6a165b6ecc948967a1ead710d4a181d7b0fbcaa183ef7ea84604994966221a",
        strip_prefix = "benchmark-1.5.0",
        urls = [
            "https://github.com/google/benchmark/archive/v1.5.0.tar.gz",
        ],
    )

    maybe(
        http_archive,
        name = "net_zlib_zlib",
        sha256 = "c3e5e9fdd5004dcb542feda5ee4f0ff0744628baf8ed2dd5d66f8ca1197cb1a1",
        strip_prefix = "zlib-1.2.11",
        urls = [
            "https://mirror.bazel.build/zlib.net/zlib-1.2.11.tar.gz",
            "https://zlib.net/zlib-1.2.11.tar.gz",
        ],
        build_file = "@com_github_jupp0r_prometheus_cpp//bazel:zlib.BUILD",
    )
