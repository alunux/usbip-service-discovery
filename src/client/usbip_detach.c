/*
 * Copyright (C) 2011 matt mooney <mfm@muteddisk.com>
 *               2005-2007 Takahiro Hirofuchi
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <ctype.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "usbip.h"
#include "usbip_common.h"
#include "usbip_network.h"
#include "vhci_driver.h"

int detach_port(const char *port)
{
    int ret = 0;
    uint8_t portnum;
    char path[PATH_MAX + 1];
    int i;
    struct usbip_imported_device *idev;
    int found = 0;

    unsigned int port_len = strlen(port);

    for (unsigned int i = 0; i < port_len; i++)
        if (!isdigit(port[i])) {
            err("invalid port %s", port);
            return -1;
        }

    portnum = atoi(port);

    ret = usbip_vhci_driver_open();
    if (ret < 0) {
        err("open vhci_driver");
        return -1;
    }

    /* check for invalid port */
    for (i = 0; i < vhci_driver->nports; i++) {
        idev = &vhci_driver->idev[i];

        if (idev->port == portnum) {
            found = 1;
            if (idev->status != VDEV_ST_NULL)
                break;
            info("Port %d is already detached!\n", idev->port);
            goto call_driver_close;
        }
    }

    if (!found) {
        err("Invalid port %s > maxports %d", port, vhci_driver->nports);
        goto call_driver_close;
    }

    /* remove the port state file */
    snprintf(path, PATH_MAX, VHCI_STATE_PATH "/port%d", portnum);

    remove(path);
    rmdir(VHCI_STATE_PATH);

    ret = usbip_vhci_detach_device(portnum);
    if (ret < 0) {
        ret = -1;
        err("Port %d detach request failed!\n", portnum);
        goto call_driver_close;
    }
    info("Port %d is now detached!\n", portnum);

call_driver_close:
    usbip_vhci_driver_close();

    return ret;
}