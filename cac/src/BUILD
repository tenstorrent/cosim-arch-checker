load("@rules_cc//cc:defs.bzl", "cc_library")
cc_library(
    name = "caccore",
    srcs = ["cacCore.cxx"],
    hdrs = [
        "cacCore.h", 
        "register.h",
        "info.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":registerBuild",
        ":infoBuild",
        ":caclibBuild"
    ],
)

cc_library(
    name = "registerBuild",
    srcs = ["register.cxx"],
    hdrs = [
        "register.h", 
        "caclib.h",
    ],
    deps = [
        ":caclibBuild"
    ]
)

cc_library(
    name = "infoBuild",
    srcs = ["info.cxx"],
    hdrs = [
        "info.h", 
        "caclib.h",
    ],
    deps = [
        ":caclibBuild"
    ]
)

cc_library(
    name = "caclibBuild",
    hdrs = [
        "caclib.h",
    ],
    deps = [
        "//cac/src/lib:cacLibInclude",
    ],    
)
