load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:utils.bzl", "maybe")

def prometheus_cpp_repositories():
    maybe(
        http_archive,
        name = "civetweb",
        strip_prefix = "civetweb-1.16",
        sha256 = "f0e471c1bf4e7804a6cfb41ea9d13e7d623b2bcc7bc1e2a4dd54951a24d60285",
        urls = [
            "https://github.com/civetweb/civetweb/archive/v1.16.tar.gz",
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
        sha256 = "6bc180a57d23d4d9515519f92b0c83d61b05b5bab188961f36ac7b06b0d9e9ce",
        strip_prefix = "benchmark-1.8.3",
        urls = [
            "https://github.com/google/benchmark/archive/v1.8.3.tar.gz",
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
        sha256 = "b21994a857a7aa6d5256ffe355c735ad4c286de44c6c81dfc04edc41a8feaeef",
        strip_prefix = "boringssl-2ff4b968a7e0cfee66d9f151cb95635b43dc1d5b",
        urls = [
            "https://github.com/google/boringssl/archive/2ff4b968a7e0cfee66d9f151cb95635b43dc1d5b.tar.gz",
            "https://storage.googleapis.com/grpc-bazel-mirror/github.com/google/boringssl/archive/2ff4b968a7e0cfee66d9f151cb95635b43dc1d5b.tar.gz",
        ],
    )

def _data_deps_extension_impl(ctx):
    prometheus_cpp_repositories()

data_deps_ext = module_extension(
    implementation = _data_deps_extension_impl,
)
