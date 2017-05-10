/*
 * Copyright (C) 2017 La Ode Muh. Fadlun Akbar <fadlun.net@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "device.h"

static int populate_usb_devices(void);
static void cleanup_device_usage(void);

struct udev* udev;
struct udev_enumerate* enumerate;
struct udev_list_entry *devices, *dev_list_entry;

static int
populate_usb_devices(void)
{
    udev = udev_new();
    if (udev == NULL) {
        printf("failed to allocate a new udev context object\n");
        return 1;
    }

    enumerate = udev_enumerate_new(udev);
    if (enumerate == NULL) {
        printf("failed to point to allocated udev monitor\n");
        return 2;
    }

    udev_enumerate_add_match_subsystem(enumerate, "usb");
    udev_enumerate_add_nomatch_sysattr(enumerate, "bDeviceClass", "09");
    udev_enumerate_add_nomatch_sysattr(enumerate, "bInterfaceNumber", NULL);
    udev_enumerate_scan_devices(enumerate);

    devices = udev_enumerate_get_list_entry(enumerate);
    if (devices == NULL) {
        printf("No USB device found, or fail to get list devices\n");
        return 3;
    }

    return 0;
}

int
total_usb_device(void)
{
    int total = 0;

    if (populate_usb_devices() != 0) {
        return 0;
    }

    udev_list_entry_foreach(dev_list_entry, devices) { total++; }

    cleanup_device_usage();
    return total;
}

static void
cleanup_device_usage(void)
{
    udev_enumerate_unref(enumerate);
    udev_unref(udev);
}

json_object*
get_devices(void)
{
    struct udev_device* dev;

    json_object* usb_contain_json = NULL;
    json_object* usb_item_json = NULL;

    const char* ret_path;
    const char* ret_attr;

    usb_contain_json = json_object_new_array();

    if (populate_usb_devices() != 0) {
        return usb_contain_json;
    }

    udev_list_entry_foreach(dev_list_entry, devices)
    {
        ret_path = udev_list_entry_get_name(dev_list_entry);
        dev = udev_device_new_from_syspath(udev, ret_path);

        usb_item_json = json_object_new_object();

        json_object_object_add(usb_item_json, "path",
                               json_object_new_string(ret_path));

        ret_attr = udev_device_get_sysattr_value(dev, "idVendor");
        json_object_object_add(usb_item_json, "idVendor",
                               json_object_new_string(ret_attr));

        ret_attr = udev_device_get_sysattr_value(dev, "idProduct");
        json_object_object_add(usb_item_json, "idProduct",
                               json_object_new_string(ret_attr));

        ret_attr = udev_device_get_sysattr_value(dev, "bConfigurationValue");
        json_object_object_add(usb_item_json, "bConfValue",
                               json_object_new_string(ret_attr));

        ret_attr = udev_device_get_sysattr_value(dev, "bNumInterfaces");
        json_object_object_add(usb_item_json, "bNumIntfs",
                               json_object_new_string(ret_attr));

        ret_attr = udev_device_get_sysname(dev);
        json_object_object_add(usb_item_json, "busid",
                               json_object_new_string(ret_attr));

        ret_attr = udev_device_get_sysattr_value(dev, "manufacturer");
        if (ret_attr != NULL) {
            json_object_object_add(usb_item_json, "manufact",
                                   json_object_new_string(ret_attr));
        } else {
            json_object_object_add(usb_item_json, "manufact",
                                   json_object_new_string("Unknown"));
        }

        ret_attr = udev_device_get_sysattr_value(dev, "product");
        if (ret_attr != NULL) {
            json_object_object_add(usb_item_json, "product",
                                   json_object_new_string(ret_attr));
        } else {
            json_object_object_add(usb_item_json, "product",
                                   json_object_new_string("Unknown"));
        }

        json_object_array_add(usb_contain_json, json_object_get(usb_item_json));
        json_object_put(usb_item_json);
        udev_device_unref(dev);
    }

    cleanup_device_usage();
    return usb_contain_json;
}