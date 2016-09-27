cc_test(
    name = "prometheus_test",
    srcs = ["counter_test.cc", "gauge_test.cc", "label_decorator_test.cc", "mock_metric.h"],
    copts = ["-Iexternal/googletest/include"],
    deps = ["@googletest//:main",
            "//lib:prometheus-cpp"],
)

cc_binary(
    name = "sample_server",
    srcs = ["sample_server.cc"],
    deps = ["//lib:prometheus-cpp"],
)
