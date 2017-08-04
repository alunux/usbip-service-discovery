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
    'usbip_bind.c',
    'utils.c',
]

monitor_src = [
    'monitor.c',
]

serve_dev_src = [
    'usbip_network.c',
    'usbipd.c',
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

extra_inc = [
    '../libs',
]

executable(
    'listen', server_src,
    c_args: server_cflags,
    dependencies: server_requires,
    include_directories: include_directories(extra_inc),
    link_with: usbip,
)

executable(
    'send_data', send_data_src,
    c_args: server_cflags,
    dependencies: server_requires,
    include_directories: include_directories(extra_inc),
    link_with: usbip,
)

executable(
    'monitor', monitor_src,
    c_args: server_cflags,
    dependencies: server_requires,
    include_directories: include_directories(extra_inc),
    link_with: usbip,
)

executable(
    'serve_dev', serve_dev_src,
    dependencies: require_libudev,
    include_directories: include_directories(extra_inc),
    link_with: usbip,
)