cc_library(
    name = "prometheus_core",
    srcs = [
        "lib/check_names.cc",
        "lib/counter.cc",
        "lib/counter_builder.cc",
        "lib/gauge.cc",
        "lib/gauge_builder.cc",
        "lib/histogram.cc",
        "lib/histogram_builder.cc",
        "lib/registry.cc",
        "lib/summary.cc",
        "lib/summary_builder.cc",
        "lib/text_serializer.cc",
    ],
    hdrs = glob(
        ["include/prometheus/*.h"],
    ),
    linkstatic = 1,
    strip_include_prefix = "include",
    visibility = ["//visibility:public"],
)

cc_library(
    name = "prometheus_cpp",
    srcs = [
        "lib/exposer.cc",
        "lib/handler.cc",
        "lib/handler.h",
    ],
    linkstatic = 1,
    strip_include_prefix = "include",
    visibility = ["//visibility:public"],
    deps = [
        ":prometheus_core",
        "@civetweb//:civetweb",
    ],
)
