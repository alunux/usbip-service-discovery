utils_src = [
    'utils.c',
    'usbip_network.c',
    'usbip_attach.c',
    'usbip_detach.c',
    'usbip_list.c',
    'usbip_bind.c',
    'usbip_unbind.c',
    'usbip_port.c',
]

tes_src = [
    'tes.c',
    'utils.c',
    'usbip_network.c',
    'usbip_attach.c',
    'usbip_detach.c',
    'usbip_list.c',
    'usbip_bind.c',
    'usbip_unbind.c',
    'usbip_port.c'
]

utils_requires = [
    require_libudev,
]

extra_inc = [
    '.',
    '../libs',
]


utils_lib = shared_library(
    'utils_lib', utils_src,
    dependencies: utils_requires,
    include_directories: include_directories(extra_inc),
    link_with: usbiplibs,
)

executable(
    'tes', tes_src,
    dependencies: utils_requires,
    include_directories: include_directories(extra_inc),
    link_with: utils_lib,
)
