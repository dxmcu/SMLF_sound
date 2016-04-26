# Bazel (http://bazel.io/) BUILD file for path_recorder.

COPTS = []
LINK_OPTS = []

# sound_server
cc_binary(
    name = "sound_server",
    srcs = [
        "src/sound_server.cpp",
    ],
    hdrs = glob([
        "include/**/**/*.hpp",
        "include/**/*.hpp",
    ]),
    includes = [
        "include",
    ],
    copts = COPTS,
    linkopts = LINKOPTS,
    linkstatic = True,
    deps = [
        "//external:roscpp",
        "//external:geometry_msgs",
        "//external:diagnostic_msgs",
    ],
    visibility = ["//visibility:public"],
)

