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
