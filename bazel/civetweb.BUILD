licenses(["notice"])  # MIT license

config_setting(
    name = "osx",
    constraint_values = [
        "@bazel_tools//platforms:osx",
    ],
)

config_setting(
    name = "windows",
    constraint_values = [
        "@bazel_tools//platforms:windows",
    ],
)

config_setting(
    name = "with_ssl",
    define_values = {
        "with_civetweb_ssl": "true",
    },
    visibility = ["//visibility:public"],
)

COPTS = [
    "-DUSE_IPV6",
    "-DNDEBUG",
    "-DNO_CGI",
    "-DNO_CACHING",
    "-DNO_FILES",
    "-UDEBUG",
] + select({
    ":with_ssl": [
        "-DOPENSSL_API_1_1",
        "-DNO_SSL_DL",
    ],
    "@//conditions:default": [
        "-DNO_SSL",
    ],
})

DEPS = select({
    ":with_ssl": [
        "@boringssl//:crypto",
        "@boringssl//:ssl",
    ],
    "@//conditions:default": [],
})

cc_library(
    name = "libcivetweb",
    srcs = [
        "src/civetweb.c",
        "src/response.inl",
    ],
    hdrs = [
        "include/civetweb.h",
    ],
    copts = COPTS,
    includes = [
        "include",
    ],
    linkopts = select({
        ":windows": [],
        "//conditions:default": ["-lpthread"],
    }) + select({
        ":osx": [],
        ":windows": [],
        "//conditions:default": ["-lrt"],
    }),
    textual_hdrs = [
        "src/md5.inl",
        "src/handle_form.inl",
    ],
    visibility = ["//visibility:public"],
    deps = DEPS,
)

cc_library(
    name = "civetweb",
    srcs = [
        "src/CivetServer.cpp",
    ],
    hdrs = [
        "include/CivetServer.h",
    ],
    copts = COPTS,
    includes = [
        "include",
    ],
    linkopts = select({
        ":windows": [],
        "//conditions:default": ["-lpthread"],
    }) + select({
        ":osx": [],
        ":windows": [],
        "//conditions:default": ["-lrt"],
    }),
    visibility = ["//visibility:public"],
    deps = [
        ":libcivetweb",
    ],
)
