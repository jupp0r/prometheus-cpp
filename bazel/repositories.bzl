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
        sha256 = "81964fe578e9bd7c94dfdb09c8e4d6e6759e19967e397dbea48d1c10e45d0df2",
        strip_prefix = "googletest-release-1.12.1",
        urls = [
            "https://github.com/google/googletest/archive/release-1.12.1.tar.gz",
        ],
    )

    maybe(
        http_archive,
        name = "com_github_curl",
        sha256 = "dfb8582a05a893e305783047d791ffef5e167d295cf8d12b9eb9cfa0991ca5a9",
        strip_prefix = "curl-7.88.0",
        urls = [
            "https://github.com/curl/curl/releases/download/curl-7_88_0/curl-7.88.0.tar.gz",
            "https://curl.haxx.se/download/curl-7.88.0.tar.gz",
        ],
        build_file = "@com_github_jupp0r_prometheus_cpp//bazel:curl.BUILD",
    )

    maybe(
        http_archive,
        name = "com_github_google_benchmark",
        sha256 = "6430e4092653380d9dc4ccb45a1e2dc9259d581f4866dc0759713126056bc1d7",
        strip_prefix = "benchmark-1.7.1",
        urls = [
            "https://github.com/google/benchmark/archive/v1.7.1.tar.gz",
        ],
    )

    maybe(
        http_archive,
        name = "zlib",
        sha256 = "b3a24de97a8fdbc835b9833169501030b8977031bcb54b3b3ac13740f846ab30",
        strip_prefix = "zlib-1.2.13",
        urls = [
            "https://mirror.bazel.build/zlib.net/zlib-1.2.13.tar.gz",
            "https://zlib.net/zlib-1.2.13.tar.gz",
            "https://storage.googleapis.com/bazel-mirror/zlib.net/zlib-1.2.13.tar.gz",
        ],
        build_file = "@com_github_jupp0r_prometheus_cpp//bazel:zlib.BUILD",
    )

    maybe(
        http_archive,
        name = "boringssl",
        # Use github mirror instead of https://boringssl.googlesource.com/boringssl
        # to obtain a boringssl archive with consistent sha256
        sha256 = "ad0b806b6c5cbd6cae121c608945d5fed468748e330632e8d53315089ad52c67",
        strip_prefix = "boringssl-6195bf8242156c9a2fa75702eee058f91b86a88b",
        urls = [
            "https://storage.googleapis.com/grpc-bazel-mirror/github.com/google/boringssl/archive/6195bf8242156c9a2fa75702eee058f91b86a88b.tar.gz",
            "https://github.com/google/boringssl/archive/6195bf8242156c9a2fa75702eee058f91b86a88b.tar.gz",
        ],
    )

def _data_deps_extension_impl(ctx):
    prometheus_cpp_repositories()

data_deps_ext = module_extension(
    implementation = _data_deps_extension_impl,
)
