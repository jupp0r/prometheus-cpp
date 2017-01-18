cc_binary(
  name = "benchmarks",
  srcs = [
       "main.cc",
       "benchmark_helpers.cc",
       "benchmark_helpers.h",
       "counter_bench.cc",
       "gauge_bench.cc",
       "histogram_bench.cc",
       "registry_bench.cc",
       ],
  deps = [
       "@googlebenchmark//:googlebenchmark",
       "//:prometheus-cpp",
       ],
  linkstatic = 1,
)
