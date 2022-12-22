licenses(["notice"])  # MIT license

config_setting(
    name = "osx",
    constraint_values = [
        "@platforms//os:osx",
    ],
)

config_setting(
    name = "windows",
    constraint_values = [
        "@platforms//os:windows",
    ],
)

cc_library(
    name = "libcivetweb",
    srcs = [
        "src/civetweb.c",
        "src/response.inl",
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
        "-UDEBUG",
    ],
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
)

cc_library(
    name = "civetweb",
    srcs = [
        "src/CivetServer.cpp",
    ],
    hdrs = [
        "include/CivetServer.h",
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
