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

def load_com_github_google_benchmark():
    native.http_archive(
        name = "com_github_google_benchmark",
        sha256 = "f8e525db3c42efc9c7f3bc5176a8fa893a9a9920bbd08cef30fb56a51854d60d",
        strip_prefix = "benchmark-1.4.1",
        urls = [
            "https://github.com/google/benchmark/archive/v1.4.1.tar.gz",
        ],
    )

def prometheus_cpp_repositories():
    load_civetweb()
    load_com_google_googletest()
    load_com_github_google_benchmark()
