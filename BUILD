# Bazel (http://bazel.io/) BUILD file for path_recorder.

COPTS = [
    "-std=c++11",
]
LINK_OPTS = []

# sound_server
cc_binary(
    name = "sound_server",
    srcs = [
        "src/sound_server.cpp",
    ]),
    includes = [
        "include",
    ],
    copts = COPTS,
    linkopts = LINK_OPTS,
    linkstatic = True,
    deps = [
        "//external:roscpp",
        "//external:geometry_msgs",
        "//external:diagnostic_msgs",
    ],
    visibility = ["//visibility:public"],
)

