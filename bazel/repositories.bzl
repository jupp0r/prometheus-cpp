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
        name = "com_google_googletest",
        sha256 = "9dc9157a9a1551ec7a7e43daea9a694a0bb5fb8bec81235d8a1e6ef64c716dcb",
        strip_prefix = "googletest-release-1.10.0",
        urls = [
            "https://github.com/google/googletest/archive/release-1.10.0.tar.gz",
        ],
    )

    maybe(
        http_archive,
        name = "com_github_curl",
        sha256 = "d4d5899a3868fbb6ae1856c3e55a32ce35913de3956d1973caccd37bd0174fa2",
        strip_prefix = "curl-7.72.0",
        urls = [
            "https://github.com/curl/curl/releases/download/curl-7_72_0/curl-7.72.0.tar.gz",
            "https://curl.haxx.se/download/curl-7.72.0.tar.gz",
        ],
        build_file = "@com_github_jupp0r_prometheus_cpp//bazel:curl.BUILD",
    )

    maybe(
        http_archive,
        name = "com_github_google_benchmark",
        sha256 = "dccbdab796baa1043f04982147e67bb6e118fe610da2c65f88912d73987e700c",
        strip_prefix = "benchmark-1.5.2",
        urls = [
            "https://github.com/google/benchmark/archive/v1.5.2.tar.gz",
        ],
    )

    maybe(
        http_archive,
        name = "com_github_tplgy_cppcodec",
        sha256 = "0edaea2a9d9709d456aa99a1c3e17812ed130f9ef2b5c2d152c230a5cbc5c482",
        strip_prefix = "cppcodec-0.2",
        urls = [
            "https://github.com/tplgy/cppcodec/archive/v0.2.tar.gz",
        ],
        build_file = "@com_github_jupp0r_prometheus_cpp//bazel:cppcodec.BUILD",
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
