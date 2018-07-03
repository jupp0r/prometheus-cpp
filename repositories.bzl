_CIVETWEB_BUILD_FILE = """
licenses(["notice"])  # MIT license

config_setting(
    name = "darwin",
    values = {"cpu": "darwin"},)

config_setting(
    name = "darwin_x86_64",
    values = {"cpu": "darwin_x86_64"},
)

config_setting(
    name = "windows",
    values = { "cpu": "x64_windows" },
)

config_setting(
    name = "windows_msvc",
    values = {"cpu": "x64_windows_msvc"},
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
    linkopts = select({
        ":windows": [],
        ":windows_msvc": [],
        "//conditions:default": ["-lpthread"],
    }) + select({
        ":darwin": [],
        ":darwin_x86_64": [],
        ":windows": [],
        ":windows_msvc": [],
        "//conditions:default": ["-lrt"],
    }),
    textual_hdrs = [
        "src/file_ops.inl",
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
    linkopts = select({
        ":windows": [],
        ":windows_msvc": [],
        "//conditions:default": ["-lpthread"],
    }) + select({
        ":darwin": [],
        ":darwin_x86_64": [],
        ":windows": [],
        ":windows_msvc": [],
        "//conditions:default": ["-lrt"],
    }),
    visibility = ["//visibility:public"],
)
"""

def load_civetweb():
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
    native.http_archive(
        name = "com_google_googletest",
        strip_prefix = "googletest-master",
        urls = [
            "https://github.com/google/googletest/archive/master.zip",
        ],
    )

def load_com_google_googlebenchmark():
    native.http_archive(
        name = "com_google_googlebenchmark",
        sha256 = "f8e525db3c42efc9c7f3bc5176a8fa893a9a9920bbd08cef30fb56a51854d60d",
        strip_prefix = "benchmark-1.4.1",
        urls = [
            "https://github.com/google/benchmark/archive/v1.4.1.tar.gz",
        ],
    )

def prometheus_cpp_repositories():
    load_civetweb()
    load_com_google_googletest()
    load_com_google_googlebenchmark()
