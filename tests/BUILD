cc_test(
    name = "prometheus_test",
    srcs = ["counter_test.cc", "gauge_test.cc", "mock_metric.h", "family_test.cc", "registry_test.cc"],
    copts = ["-Iexternal/googletest/include"],
    deps = ["@googletest//:main",
            "//lib:prometheus-cpp"],
            linkstatic = 1,
)

cc_binary(
    name = "sample_server",
    srcs = ["sample_server.cc"],
    deps = ["//lib:prometheus-cpp"],
)
