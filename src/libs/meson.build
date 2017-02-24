sources = [
    'names.c',
    'usbip_host_driver.c',
    'usbip_common.c',
    'vhci_driver.c',
    'sysfs_utils.c',
]

deps = [
    require_libudev
]

ver = '0.1.0'

usbiplibs = shared_library(
    'usbiplibs', sources,
    version : ver,
    dependencies : deps
)
