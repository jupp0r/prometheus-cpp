new_git_repository(
    name = "googletest",
    remote = "https://github.com/google/googletest.git",
    tag = "release-1.8.0",
    build_file_content = """
cc_library(
    name = "gmock",
    srcs = [
        "googletest/src/gtest-all.cc",
        "googlemock/src/gmock-all.cc",
    ],
    hdrs = glob([
        "googletest/**/*.h",
        "googlemock/**/*.h",
        "googletest/src/*.cc",
        "googlemock/src/*.cc",
    ]),
    includes = [
        "googlemock",
        "googletest",
        "googletest/include",
        "googlemock/include",
    ],
    linkopts = ["-pthread"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "main",
    srcs = ["googlemock/src/gmock_main.cc"],
    linkopts = ["-pthread"],
    visibility = ["//visibility:public"],
    deps = [":gmock"],
)

""",
)

new_git_repository(
    name = "prometheus_client_model",
    remote = "https://github.com/prometheus/client_model.git",
    commit = "e2da43a",
    build_file_content = """
cc_library(
    name = "prometheus_client_model",
    srcs = [
        "cpp/metrics.pb.cc",
    ],
    hdrs = [
         "cpp/metrics.pb.h",
    ],
    includes = [
         "cpp",
    ],
    visibility = ["//visibility:public"],
    deps = ["@protobuf//:protobuf"],
)
    """,
)

git_repository(
    name = "protobuf",
    remote = "https://github.com/google/protobuf.git",
    tag = "v3.0.0",
    )

new_git_repository(
    name = "civetweb",
    remote = "https://github.com/civetweb/civetweb.git",
    commit = "fbdee74",
    build_file_content = """
cc_library(
    name = "civetweb",
    srcs = [
         "src/civetweb.c",
         "src/CivetServer.cpp",
    ],
    hdrs = [
         "include/civetweb.h",
         "include/CivetServer.h",
         "src/md5.inl",
         "src/handle_form.inl",
    ],
    includes = [
         "include",
    ],
    copts = [
          "-DUSE_IPV6",
          "-DNDEBUG",
          "-DNO_CGI",
          "-DNO_CACHING",
          "-DNO_SSL",
          "-DNO_FILES",
    ],
    visibility = ["//visibility:public"],
)
"""
)

new_git_repository(
    name = "googlebenchmark",
    remote = "https://github.com/google/benchmark.git",
    commit = "57a22c69b382b3f010ec4042c9574ea3fd8dcbb4",
    build_file_content = """
cc_library(
    name = "googlebenchmark",
    srcs = glob(["src/*.cc"],
                exclude = ["src/re_posix.cc", "src/gnuregex.cc"]),
    hdrs = glob(["src/*.h", "include/benchmark/*.h"],
                exclude = ["src/re_posix.h", "src/gnuregex.h"]),
    includes = [
         "include",
    ],
    visibility = ["//visibility:public"],
    copts = [
          "-DHAVE_STD_REGEX"
    ],
)
"""
)
