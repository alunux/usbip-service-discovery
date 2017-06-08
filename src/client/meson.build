nekofi_res = gnome.compile_resources(
  'nekofi-res', 'nekofi.gresource.xml',
  source_dir : '.',
  c_name : 'nekofi'
)

frontend_src = [
    'main.c',
    'broadcast_event.c',
    'discover.c',
    'nekofi.c',
    'nekofiwin.c',
    'usbip_attach.c',
    'usbip_detach.c',
    'usbip_network.c',
    nekofi_res,
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
    'main', frontend_src,
    c_args: frontend_cflags,
    dependencies: frontend_requires,
    include_directories: include_directories(extra_inc),
    link_with: usbip,
)