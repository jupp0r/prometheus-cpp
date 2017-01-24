cc_binary(
    name = "sample-server",
    srcs = ["sample_server.cc"],
    deps = ["//:prometheus-cpp"],
)

sh_test(
    name = "scrape-test",
    size = "small",
    srcs = ["scrape.sh"],
    data = [
        "sample-server",
        "scrape.conf",
    ],
)
