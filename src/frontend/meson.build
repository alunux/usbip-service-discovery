frontend_src = [
    'main.c',
]

frontend_requires = [
    require_frontend,
    require_libudev,
]

frontend_cflags = [
    '-O2',
    '-D_FORTIFY_SOURCE=2',
    '-fstack-protector',
    '-pedantic',
    '-Wstrict-prototypes',
    '-Wundef',
    '-fno-common',
    '-Werror-implicit-function-declaration',
    '-Wformat',
    '-Wformat-security',
    '-Werror=format-security',
    '-Wno-conversion',
    '-Wunused-variable',
    '-Wunreachable-code',
]

extra_incl = [
    '../libs',
    '../utils',
]

executable(
    'main', frontend_src,
    c_args: frontend_cflags,
    include_directories: include_directories(extra_incl),
    dependencies: frontend_requires,
    link_with: utils_lib,
)
