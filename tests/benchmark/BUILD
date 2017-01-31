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
    ],
    linkstatic = 1,
    deps = [
        "//:prometheus-cpp",
        "@googlebenchmark//:googlebenchmark",
    ],
)
