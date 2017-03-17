frontend_src = [
    'main.c',
]

frontend_requires = [
    require_frontend,
    require_libudev,
]

extra_incl = [
    '../libs',
    '../utils',
]

executable(
    'main', frontend_src,
    include_directories: include_directories(extra_incl),
    dependencies: frontend_requires,
    link_with: utils_lib,
)
