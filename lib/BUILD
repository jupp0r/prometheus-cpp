cc_library(
    name = "prometheus-cpp",
    srcs = ["counter.cc",
            "gauge.cc",
            "exposer.cc"],
    hdrs = ["counter.h",
            "gauge.h",
            "exposer.h"],
    visibility = ["//visibility:public"],
    deps = ["@protobuf//:protobuf",
            "@prometheus_client_model//:prometheus_client_model",
            "@civetweb//:civetweb",
           ],
)
