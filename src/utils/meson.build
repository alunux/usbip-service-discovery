utils_src = [
    'utils.c',
    'usbip_network.c',
    'usbip_attach.c',
    'usbip_detach.c',
    'usbip_bind.c',
    'usbip_unbind.c',
]

tes_src = [
    'tes.c',
    'utils.c',
    'usbip_network.c',
    'usbip_attach.c',
    'usbip_detach.c',
    'usbip_bind.c',
    'usbip_unbind.c',
]

neko_serve = [
    'usbip_network.c',
    'usbipd.c',
]

utils_requires = [
    require_libudev,
]

extra_inc = [
    '.',
    '../libs',
]


libusbip_util = shared_library(
    'libusbip_util', utils_src,
    dependencies: utils_requires,
    include_directories: include_directories(extra_inc),
    link_with: libusbip,
)

executable(
    'nekofi-serve', neko_serve,
    dependencies: utils_requires,
    include_directories: include_directories(extra_inc),
    link_with: libusbip_util,
)
