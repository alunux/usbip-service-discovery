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

#ifndef DEVICE_H
#define DEVICE_H

#include <errno.h>
#include <glib.h>
#include <libudev.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

typedef struct _UsbDevice {
    const char* path;
    const char* idVendor;
    const char* idProduct;
    const char* bConfValue;
    const char* bNumIntfs;
    const char* busid;
    const char* manufact;
    const char* product_usb;
    struct udev_device* dev;
} UsbDevice;

GSList* usb_devices_list(void);
int total_usb_device(void);
void finish_dev_usage(GSList* free_list);

#endif /* DEVICE_H */
