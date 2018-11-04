def load_civetweb():
    native.new_http_archive(
        name = "civetweb",
        strip_prefix = "civetweb-1.9.1",
        sha256 = "880d741724fd8de0ebc77bc5d98fa673ba44423dc4918361c3cd5cf80955e36d",
        urls = [
            "https://github.com/civetweb/civetweb/archive/v1.9.1.tar.gz",
        ],
        build_file = "@com_github_jupp0r_prometheus_cpp//bazel:civetweb.BUILD",
    )

def load_com_google_googletest():
    native.http_archive(
        name = "com_google_googletest",
        strip_prefix = "googletest-master",
        urls = [
            "https://github.com/google/googletest/archive/master.zip",
        ],
    )

def load_com_github_curl():
    native.new_http_archive(
        name = "com_github_curl",
        sha256 = "e9c37986337743f37fd14fe8737f246e97aec94b39d1b71e8a5973f72a9fc4f5",
        strip_prefix = "curl-7.60.0",
        urls = [
            "https://mirror.bazel.build/curl.haxx.se/download/curl-7.60.0.tar.gz",
            "https://curl.haxx.se/download/curl-7.60.0.tar.gz",
        ],
        build_file = "@com_github_jupp0r_prometheus_cpp//bazel:curl.BUILD",
    )

def load_com_github_google_benchmark():
    native.http_archive(
        name = "com_github_google_benchmark",
        sha256 = "f8e525db3c42efc9c7f3bc5176a8fa893a9a9920bbd08cef30fb56a51854d60d",
        strip_prefix = "benchmark-1.4.1",
        urls = [
            "https://github.com/google/benchmark/archive/v1.4.1.tar.gz",
        ],
    )

def load_com_github_madler_zlib():
    native.new_http_archive(
        name = "com_github_madler_zlib",
        sha256 = "629380c90a77b964d896ed37163f5c3a34f6e6d897311f1df2a7016355c45eff",
        strip_prefix = "zlib-1.2.11",
        urls = [
            "https://github.com/madler/zlib/archive/v1.2.11.tar.gz",
        ],
        build_file = "@com_github_jupp0r_prometheus_cpp//bazel:zlib.BUILD",
    )

def prometheus_cpp_repositories():
    load_civetweb()
    load_com_google_googletest()
    load_com_github_curl()
    load_com_github_google_benchmark()
    load_com_github_madler_zlib()
