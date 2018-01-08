_PROMETHEUS_CLIENT_MODEL_BUILD_FILE = """
licenses(["notice"])  # BSD license

load("@com_google_protobuf//:protobuf.bzl", "cc_proto_library")

cc_proto_library(
    name = "prometheus_client_model",
    srcs = ["metrics.proto"],
    default_runtime = "@com_google_protobuf//:protobuf",
    protoc = "@com_google_protobuf//:protoc",
    visibility = ["//visibility:public"],
)
"""

_CIVETWEB_BUILD_FILE = """
licenses(["notice"])  # MIT license

cc_library(
    name = "civetweb",
    srcs = [
        "src/CivetServer.cpp",
        "src/civetweb.c",
    ],
    hdrs = [
        "include/CivetServer.h",
        "include/civetweb.h",
    ],
    copts = [
        "-DUSE_IPV6",
        "-DNDEBUG",
        "-DNO_CGI",
        "-DNO_CACHING",
        "-DNO_SSL",
        "-DNO_FILES",
    ],
    includes = [
        "include",
    ],
    textual_hdrs = [
        "src/md5.inl",
        "src/handle_form.inl",
    ],
    visibility = ["//visibility:public"],
)
"""

_GOOGLEBENCHEMARK_BUILD_FILE = """
licenses(["notice"])  # Apache-2.0 license

cc_library(
    name = "googlebenchmark",
    srcs = glob(
        ["src/*.cc"],
        exclude = [
            "src/re_posix.cc",
            "src/gnuregex.cc",
        ],
    ),
    hdrs = glob(
        [
            "src/*.h",
            "include/benchmark/*.h",
        ],
        exclude = [
            "src/re_posix.h",
            "src/gnuregex.h",
        ],
    ),
    copts = [
        "-DHAVE_STD_REGEX",
    ],
    includes = [
        "include",
    ],
    visibility = ["//visibility:public"],
)
"""

def load_civetweb():
    native.new_http_archive(
        name = "civetweb",
        strip_prefix = "civetweb-1.9.1",
        sha256 = "880d741724fd8de0ebc77bc5d98fa673ba44423dc4918361c3cd5cf80955e36d",
        urls = [
           "https://github.com/civetweb/civetweb/archive/v1.9.1.tar.gz",
       ],
       build_file_content = _CIVETWEB_BUILD_FILE,
    )

def load_prometheus_client_model():
    native.new_git_repository(
        name = "prometheus_client_model",
        remote = "https://github.com/prometheus/client_model.git",
        commit = "e2da43ae71fe22f457da00bb0b1f4fcaec9113c2",
        build_file_content = _PROMETHEUS_CLIENT_MODEL_BUILD_FILE,
    )

def load_com_google_protobuf():
    native.http_archive(
        name = "com_google_protobuf",
        sha256 = "8e0236242106e680b4f9f576cc44b8cd711e948b20a9fc07769b0a20ceab9cc4",
        strip_prefix = "protobuf-3.4.1",
        urls = [
            "https://github.com/google/protobuf/archive/v3.4.1.tar.gz",
        ],
    )

def load_com_google_googletest():
    native.http_archive(
        name = "com_google_googletest",
        strip_prefix = "googletest-master",
        urls = [
            "https://github.com/google/googletest/archive/master.zip",
        ],
    )

def load_com_google_googlebenchmark():
    native.new_http_archive(
        name = "com_google_googlebenchmark",
        sha256 = "3dcc90c158838e2ac4a7ad06af9e28eb5877cf28252a81e55eb3c836757d3070",
        strip_prefix = "benchmark-1.2.0",
        urls = [
            "https://github.com/google/benchmark/archive/v1.2.0.tar.gz",
        ],
        build_file_content = _GOOGLEBENCHEMARK_BUILD_FILE,
    )

def prometheus_cpp_repositories():
    load_com_google_protobuf()
    load_prometheus_client_model()
    load_civetweb()
    load_com_google_googletest()
    load_com_google_googlebenchmark()
