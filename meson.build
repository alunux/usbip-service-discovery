project('skripsi-labora', 'c', default_options: 'c_std=gnu11', version : '2.0')
build_server = get_option('build-server')
build_client = get_option('build-client')

if build_client == true
    gnome = import('gnome')
    require_frontend = dependency('gtk+-3.0')
    require_glib = dependency('glib-2.0')
endif

require_libudev = dependency('libudev')
require_json = dependency('json-c')

am_cppflags = [
    '-DUSBIDS_FILE="/usr/share/hwdata/usb.ids"'
]

am_cflags = [
    '-Wall',
    '-Wextra',
]

add_global_arguments(am_cflags, language: 'c')
add_global_arguments(am_cppflags, language: 'c')

cc = meson.get_compiler('c')
cdata = configuration_data()

cdata.set('USBIP_VERSION', '0x00000111')
cdata.set_quoted('PACKAGE', 'usbip-utils')
cdata.set_quoted('PACKAGE_STRING', 'usbip-utils 2.0')
cdata.set_quoted('PACKAGE_TARNAME', 'usbip-utils')
cdata.set_quoted('PACKAGE_URL', '')
cdata.set_quoted('PACKAGE_VERSION', meson.project_version())
cdata.set_quoted('VERSION', meson.project_version())

check_headers = [
    ['HAVE_ARPA_INET_H', 'arpa/inet.h'],
    ['HAVE_DIRENT_H', 'dirent.h'],
    ['HAVE_DLFCN_H', 'dlfcn.h'],
    ['HAVE_FCNTL_H', 'fcntl.h'],
    ['HAVE_INTTYPES_H', 'inttypes.h'],
    ['HAVE_LIBUSB_1_0_LIBUSB_H', 'libusb-1.0/libusb.h'],
    ['HAVE_MEMORY_H', 'memory.h'],
    ['HAVE_NDIR_H', 'ndir.h'],
    ['HAVE_NETDB_H', 'netdb.h'],
    ['HAVE_NETINET_IN_H', 'netinet/in.h'],
    ['HAVE_STDINT_H', 'stdint.h'],
    ['HAVE_STDLIB_H', 'stdlib.h'],
    ['HAVE_STRINGS_H', 'strings.h'],
    ['HAVE_STRING_H', 'string.h'],
    ['HAVE_SYSLOG_H', 'syslog.h'],
    ['HAVE_SYS_DIR_H', 'sys/dir.h'],
    ['HAVE_SYS_NDIR_H', 'sys/ndir.h'],
    ['HAVE_SYS_SOCKET_H', 'sys/socket.h'],
    ['HAVE_SYS_STAT_H', 'sys/stat.h'],
    ['HAVE_SYS_TYPES_H', 'sys/types.h'],
    ['HAVE_UNISTD_H', 'unistd.h'],
]

foreach h : check_headers
    if cc.has_header(h.get(1))
        cdata.set(h.get(0), 1)
    endif
endforeach

check_functions = [
# check token ('HAVE_DNL',)
# check token ('HAVE_LIBWRAP',)
    ['HAVE_MEMSET', 'memset', '#include<string.h>'],
# check token ('HAVE_MKDIR',)
    ['HAVE_REALLOC', 'realloc', '#include<stdlib.h>'],
# check token ('HAVE_REGCOMP',)
    ['HAVE_SOCKET', 'socket', '#include<sys/socket.h>'],
    ['HAVE_STRCHR', 'strchr', '#include<string.h>'],
    ['HAVE_STRERROR', 'strerror', '#include<string.h>'],
    ['HAVE_STRSTR', 'strstr', '#include<string.h>'],
    ['HAVE_STRTOUL', 'strtoul', '#include<stdlib.h>'],
]

foreach f : check_functions
if cc.has_function(f.get(1), prefix : f.get(2))
    cdata.set(f.get(0), 1)
    endif
endforeach

configure_file(
    input : 'config.h.meson',
    output : 'config.h',
    configuration : cdata
)

top_inc = include_directories('.')

subdir('src')
subdir('data')
