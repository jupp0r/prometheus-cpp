cc_test(
    name = "prometheus-test",
    srcs = [
        "check_names_test.cc",
        "counter_test.cc",
        "family_test.cc",
        "gauge_test.cc",
        "histogram_test.cc",
        "mock_metric.h",
        "registry_test.cc",
        "summary_test.cc",
    ],
    copts = ["-Iexternal/googletest/include"],
    linkstatic = 1,
    deps = [
        "//:prometheus_cpp",
        "@com_google_googletest//:gtest_main",
    ],
)
