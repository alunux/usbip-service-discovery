usbip_app_res = gnome.compile_resources(
  'usbip_app-res', 'usbip_app.gresource.xml',
  source_dir : '.',
  c_name : 'usbip_app'
)

frontend_src = [
    'main.c',
    'multicast_event.c',
    'discover.c',
    'usb_desc.c',
    'usbip_app.c',
    'usbip_app_win.c',
    'usbip_attach.c',
    'usbip_detach.c',
    'usbip_network.c',
    usbip_app_res,
]

frontend_pkexec_src = [
    'usbip_app_pkexec.c',
]

frontend_requires = [
    require_frontend,
    require_libudev,
    require_json,
]

frontend_cflags = [
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
    'usbip-app-client', frontend_src,
    c_args: frontend_cflags,
    dependencies: frontend_requires,
    include_directories: include_directories(extra_inc),
    link_with: usbip,
    install : true,
)

executable(
    'usbip-app-pkexec', frontend_pkexec_src,
    c_args: frontend_cflags,
    dependencies: require_glib,
    install : true,
)