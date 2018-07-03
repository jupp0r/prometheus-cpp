cc_binary(
    name = "benchmarks",
    srcs = [
        "benchmark_helpers.cc",
        "benchmark_helpers.h",
        "counter_bench.cc",
        "gauge_bench.cc",
        "histogram_bench.cc",
        "main.cc",
        "registry_bench.cc",
        "summary_bench.cc",
    ],
    linkstatic = 1,
    deps = [
        "//:prometheus_core",
        "@com_google_googlebenchmark//:googlebenchmark",
    ],
)
