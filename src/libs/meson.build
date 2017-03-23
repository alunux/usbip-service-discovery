sources = [
    'names.c',
    'usbip_host_driver.c',
    'usbip_common.c',
    'vhci_driver.c',
    'sysfs_utils.c',
]

extra_cflags = [
    '-Wno-variadic-macros',
]

deps = [
    require_libudev
]

ver = '0.1.0'

usbiplibs = shared_library(
    'usbiplibs', sources,
    c_args: extra_cflags,
    version : ver,
    dependencies : deps
)
