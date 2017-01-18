cc_test(
    name = "prometheus-test",
    srcs = ["counter_test.cc", "gauge_test.cc", "mock_metric.h", "family_test.cc", "registry_test.cc", "histogram_test.cc"],
    copts = ["-Iexternal/googletest/include"],
    deps = ["@googletest//:main",
            "//:prometheus-cpp"],
            linkstatic = 1,
)
