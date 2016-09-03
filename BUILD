# Bazel (http://bazel.io/) BUILD file for path_recorder.

COPTS = [
    "-std=c++11",
]
LINK_OPTS = []

# sound_server
cc_binary(
    name = "sound_server",
    srcs = glob([
        "src/sound_server.cpp",
        "include/**/*.hpp",
    ]),
    includes = [
        "include/**/*.h",
    ],
    copts = COPTS,
    linkopts = LINK_OPTS + [
        "-lsfml-system",
        "-lsfml-audio",
    ],
    linkstatic = True,
    deps = [
        "@io_bazel_rules_ros//ros:diagnostic_msgs",
        "@io_bazel_rules_ros//ros:roscpp",
        "@io_bazel_rules_ros//ros:std_msgs",
    ],
    visibility = ["//visibility:public"],

)

