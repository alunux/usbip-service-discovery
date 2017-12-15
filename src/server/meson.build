server_src = [
    'discovery_listener.c',
]

server_requires = [
    require_libudev,
    require_json,
]

usb_metadata_server_src = [
    'usb_metadata_server.c',
    'device.c',
    'usbip_bind.c',
    'usbip_unbind.c',
    'utils.c',
]

monitor_src = [
    'usb_event_monitor.c',
]

serve_dev_src = [
    'usbip_network.c',
    'usbipd.c',
]

server_cflags = [
    '-O2',
    '-D_FORTIFY_SOURCE=2',
    '-fstack-protector',
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
    'discovery_listener', server_src,
    c_args: server_cflags,
    dependencies: server_requires,
    include_directories: include_directories(extra_inc),
    link_with: usbip,
    install: true,
)

executable(
    'usb_metadata_server', usb_metadata_server_src,
    c_args: server_cflags,
    dependencies: server_requires,
    include_directories: include_directories(extra_inc),
    link_with: usbip,
    install: true,
)

executable(
    'usb_event_monitor', monitor_src,
    c_args: server_cflags,
    dependencies: server_requires,
    include_directories: include_directories(extra_inc),
    link_with: usbip,
    install: true,
)

executable(
    'usbip_daemon', serve_dev_src,
    dependencies: require_libudev,
    include_directories: include_directories(extra_inc),
    link_with: usbip,
    install: true,
    install_dir: get_option('sbindir'),
)