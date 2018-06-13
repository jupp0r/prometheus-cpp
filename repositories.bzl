_CIVETWEB_BUILD_FILE = """
licenses(["notice"])  # MIT license

config_setting(
    name = "darwin",
    values = {"cpu": "darwin"},)

config_setting(
    name = "darwin_x86_64",
    values = {"cpu": "darwin_x86_64"},
)

cc_library(
    name = "libcivetweb",
    srcs = [
        "src/civetweb.c",
    ],
    hdrs = [
        "include/civetweb.h",
    ],
    copts = [
        "-DUSE_IPV6",
        "-DNDEBUG",
        "-DNO_CGI",
        "-DNO_CACHING",
        "-DNO_SSL",
        "-DNO_FILES",
    ],
    includes = [
        "include",
    ],
    linkopts = [
        "-lpthread",
    ] + select({
        ":darwin": [],
        ":darwin_x86_64": [],
        "//conditions:default": ["-lrt"],
    }),
    textual_hdrs = [
        "src/md5.inl",
        "src/handle_form.inl",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "civetweb",
    srcs = [
        "src/CivetServer.cpp",
    ],
    hdrs = [
        "include/CivetServer.h",
    ],
    deps = [
        ":libcivetweb",
    ],
    copts = [
        "-DUSE_IPV6",
        "-DNDEBUG",
        "-DNO_CGI",
        "-DNO_CACHING",
        "-DNO_SSL",
        "-DNO_FILES",
    ],
    includes = [
        "include",
    ],
    linkopts = [
        "-lpthread",
    ] + select({
        ":darwin": [],
        ":darwin_x86_64": [],
        "//conditions:default": ["-lrt"],
    }),
    visibility = ["//visibility:public"],
)
"""

_GOOGLEBENCHEMARK_BUILD_FILE = """
licenses(["notice"])  # Apache-2.0 license

cc_library(
    name = "googlebenchmark",
    srcs = glob(
        ["src/*.cc"],
        exclude = [
            "src/re_posix.cc",
            "src/gnuregex.cc",
        ],
    ),
    hdrs = glob(
        [
            "src/*.h",
            "include/benchmark/*.h",
        ],
        exclude = [
            "src/re_posix.h",
            "src/gnuregex.h",
        ],
    ),
    copts = [
        "-DHAVE_STD_REGEX",
    ],
    includes = [
        "include",
    ],
    visibility = ["//visibility:public"],
)
"""

def load_civetweb():
    if "civetweb" not in native.existing_rules():
        native.new_http_archive(
            name = "civetweb",
            strip_prefix = "civetweb-1.9.1",
            sha256 = "880d741724fd8de0ebc77bc5d98fa673ba44423dc4918361c3cd5cf80955e36d",
            urls = [
            "https://github.com/civetweb/civetweb/archive/v1.9.1.tar.gz",
        ],
        build_file_content = _CIVETWEB_BUILD_FILE,
        )

def load_com_google_googletest():
    if "com_google_googletest" not in native.existing_rules():
        native.http_archive(
            name = "com_google_googletest",
            strip_prefix = "googletest-master",
            urls = [
                "https://github.com/google/googletest/archive/master.zip",
            ],
        )

def load_com_google_googlebenchmark():
    if "com_google_googlebenchmark" not in native.existing_rules():
        native.new_http_archive(
            name = "com_google_googlebenchmark",
            sha256 = "3dcc90c158838e2ac4a7ad06af9e28eb5877cf28252a81e55eb3c836757d3070",
            strip_prefix = "benchmark-1.2.0",
            urls = [
                "https://github.com/google/benchmark/archive/v1.2.0.tar.gz",
            ],
            build_file_content = _GOOGLEBENCHEMARK_BUILD_FILE,
        )

def prometheus_cpp_repositories():
    load_civetweb()
    load_com_google_googletest()
    load_com_google_googlebenchmark()
