cc_binary(
    name = "sample_server",
    srcs = ["sample_server.cc"],
    deps = ["//lib:prometheus-cpp"],
)

sh_test(
    name = "scrape_test",
    srcs = ["scrape.sh"],
    size = "small",
    data = ["scrape.conf", "sample_server"],
)
