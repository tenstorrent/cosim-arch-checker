load("@rules_cc//cc:defs.bzl", "cc_test")

cc_test(
    name = 'basicClassTest',
    srcs = ["basicClassTest.cxx"],
    deps = [
        "//src:caccore",
        "@googletest//:gtest_main",
    ]
)

cc_test(
    name = 'testCacCore',
    srcs = ["testCacCore.cxx"],
    deps = [
        "//src:caccore",
        "@googletest//:gtest_main",
    ]
)