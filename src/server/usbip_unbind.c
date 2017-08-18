/*
 * Copyright (C) 2017 La Ode Muh. Fadlun Akbar <fadlun.net@gmail.com>
 *               2011 matt mooney <mfm@muteddisk.com>
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

#include <libudev.h>

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <getopt.h>

#include "sysfs_utils.h"
#include "usbip.h"
#include "usbip_common.h"
#include "utils.h"

int
unbind_device(char* busid)
{
    char bus_type[] = "usb";
    int rc, ret = -1;

    char unbind_attr_name[] = "unbind";
    char unbind_attr_path[SYSFS_PATH_MAX];
    char rebind_attr_name[] = "rebind";
    char rebind_attr_path[SYSFS_PATH_MAX];

    struct udev* udev;
    struct udev_device* dev;
    const char* driver;

    /* Create libudev context. */
    udev = udev_new();

    /* Check whether the device with this bus ID exists. */
    dev = udev_device_new_from_subsystem_sysname(udev, "usb", busid);
    if (!dev) {
        err("device with the specified bus ID does not exist");
        goto err_close_udev;
    }

    /* Check whether the device is using usbip-host driver. */
    driver = udev_device_get_driver(dev);
    if (!driver || strcmp(driver, "usbip-host")) {
        err("device is not bound to usbip-host driver");
        goto err_close_udev;
    }

    /* Unbind device from driver. */
    snprintf(unbind_attr_path,
             sizeof(unbind_attr_path),
             "%s/%s/%s/%s/%s/%s",
             SYSFS_MNT_PATH,
             SYSFS_BUS_NAME,
             bus_type,
             SYSFS_DRIVERS_NAME,
             USBIP_HOST_DRV_NAME,
             unbind_attr_name);

    rc = write_sysfs_attribute(unbind_attr_path, busid, strlen(busid));
    if (rc < 0) {
        err("error unbinding device %s from driver", busid);
        goto err_close_udev;
    }

    /* Notify driver of unbind. */
    rc = modify_match_busid(busid, 0);
    if (rc < 0) {
        err("unable to unbind device on %s", busid);
        goto err_close_udev;
    }

    /* Trigger new probing. */
    snprintf(rebind_attr_path,
             sizeof(unbind_attr_path),
             "%s/%s/%s/%s/%s/%s",
             SYSFS_MNT_PATH,
             SYSFS_BUS_NAME,
             bus_type,
             SYSFS_DRIVERS_NAME,
             USBIP_HOST_DRV_NAME,
             rebind_attr_name);

    rc = write_sysfs_attribute(rebind_attr_path, busid, strlen(busid));
    if (rc < 0) {
        err("error rebinding");
        goto err_close_udev;
    }

    ret = 0;
    info("unbind device on busid %s: complete", busid);

err_close_udev:
    udev_device_unref(dev);
    udev_unref(udev);

    return ret;
}