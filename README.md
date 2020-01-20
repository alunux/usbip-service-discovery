# USB/IP Service Discovery
Provide GUI and some enhancement for USB/IP userspace. It includes a mechanism
which utilizes existing approaches to the pervasive computing and IoT. IP Multicast
is one of the methods used in USB/IP Service Discovery to find USB/IP service in your local area.

It's still under development, nothing is useful here.

### Build Requirements
- gcc >= 4.0
- pkg-config
- meson >= 0.29
- libudev >= 229
- glib-2.0 >= 2.44.0
- gtk+-3.0 >= 3.18.0
- json-c >= 0.11

### Building
#### Building the Client
in the checkout run the following sequence
`meson build-client`
`ninja -C build-client install`

#### Building the Server
in the checkout run the following sequence
`meson build-server`
`ninja -C build-server install`


### Screenshot

<p align=center>
    <img src="https://github.com/alunux/usbip-gui-utils/raw/master/.github/screenshots/main.png">
</p>

### Thanks To
Thanks to The USB/IP Project (Takahiro Hirofuchi, et al.) and the Linux kernel developers.

### LICENSE
Copyright © 2017-2018 La Ode Muh. Fadlun Akbar

usbip-service-discovery is available under the terms of the GPL-2.0-or-later license.

See the accompanying LICENSE file for more details
