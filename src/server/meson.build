server_src = [
    'listen.c',
]

server_requires = [
    require_libudev,
    require_json,
]

send_data_src = [
    'send_data.c',
    'device.c',
]

monitor_src = [
    'monitor.c'
]

server_cflags = [
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

executable(
    'listen', server_src,
    c_args: server_cflags,
    dependencies: server_requires,
)

executable(
    'send_data', send_data_src,
    c_args: server_cflags,
    dependencies: server_requires,
)

executable(
    'monitor', monitor_src,
    c_args: server_cflags,
    dependencies: server_requires,
)