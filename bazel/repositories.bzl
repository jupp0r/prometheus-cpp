load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:utils.bzl", "maybe")

def prometheus_cpp_repositories():
    maybe(
        http_archive,
        name = "civetweb",
        strip_prefix = "civetweb-1.15",
        sha256 = "90a533422944ab327a4fbb9969f0845d0dba05354f9cacce3a5005fa59f593b9",
        urls = [
            "https://github.com/civetweb/civetweb/archive/v1.15.tar.gz",
        ],
        build_file = "@com_github_jupp0r_prometheus_cpp//bazel:civetweb.BUILD",
    )

    maybe(
        http_archive,
        name = "com_google_googletest",
        sha256 = "b4870bf121ff7795ba20d20bcdd8627b8e088f2d1dab299a031c1034eddc93d5",
        strip_prefix = "googletest-release-1.11.0",
        urls = [
            "https://github.com/google/googletest/archive/release-1.11.0.tar.gz",
        ],
    )

    maybe(
        http_archive,
        name = "com_github_curl",
        sha256 = "ed936c0b02c06d42cf84b39dd12bb14b62d77c7c4e875ade022280df5dcc81d7",
        strip_prefix = "curl-7.78.0",
        urls = [
            "https://github.com/curl/curl/releases/download/curl-7_78_0/curl-7.78.0.tar.gz",
            "https://curl.haxx.se/download/curl-7.78.0.tar.gz",
        ],
        build_file = "@com_github_jupp0r_prometheus_cpp//bazel:curl.BUILD",
    )

    maybe(
        http_archive,
        name = "com_github_google_benchmark",
        sha256 = "1f71c72ce08d2c1310011ea6436b31e39ccab8c2db94186d26657d41747c85d6",
        strip_prefix = "benchmark-1.6.0",
        urls = [
            "https://github.com/google/benchmark/archive/v1.6.0.tar.gz",
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
