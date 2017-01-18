cc_binary(
    name = "sample-server",
    srcs = ["sample_server.cc"],
    deps = ["//:prometheus-cpp"],
)

sh_test(
    name = "scrape-test",
    srcs = ["scrape.sh"],
    size = "small",
    data = ["scrape.conf", "sample-server"],
)
