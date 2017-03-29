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
#include <sys/types.h>

#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <getopt.h>
#include <netdb.h>
#include <unistd.h>

#include "usbip.h"
#include "usbip_common.h"
#include "usbip_network.h"

static int
get_exported_devices(char* host, int sockfd)
{
    char product_name[100];
    char class_name[100];
    struct op_devlist_reply reply;
    uint16_t code = OP_REP_DEVLIST;
    struct usbip_usb_device udev;
    struct usbip_usb_interface uintf;
    unsigned int i;
    int rc, j;

    rc = usbip_net_send_op_common(sockfd, OP_REQ_DEVLIST, 0);
    if (rc < 0) {
        dbg("usbip_net_send_op_common failed");
        return -1;
    }

    rc = usbip_net_recv_op_common(sockfd, &code);
    if (rc < 0) {
        dbg("usbip_net_recv_op_common failed");
        return -1;
    }

    memset(&reply, 0, sizeof(reply));
    rc = usbip_net_recv(sockfd, &reply, sizeof(reply));
    if (rc < 0) {
        dbg("usbip_net_recv_op_devlist failed");
        return -1;
    }
    PACK_OP_DEVLIST_REPLY(0, &reply);
    dbg("exportable devices: %d\n", reply.ndev);

    if (reply.ndev == 0) {
        info("no exportable devices found on %s", host);
        return 0;
    }

    printf("Exportable USB devices\n");
    printf("======================\n");
    printf(" - %s\n", host);

    for (i = 0; i < reply.ndev; i++) {
        memset(&udev, 0, sizeof(udev));
        rc = usbip_net_recv(sockfd, &udev, sizeof(udev));
        if (rc < 0) {
            dbg("usbip_net_recv failed: usbip_usb_device[%d]", i);
            return -1;
        }
        usbip_net_pack_usb_device(0, &udev);

        usbip_names_get_product(product_name, sizeof(product_name),
                                udev.idVendor, udev.idProduct);
        usbip_names_get_class(class_name, sizeof(class_name), udev.bDeviceClass,
                              udev.bDeviceSubClass, udev.bDeviceProtocol);
        printf("%11s: %s\n", udev.busid, product_name);
        printf("%11s: %s\n", "", udev.path);
        printf("%11s: %s\n", "", class_name);

        for (j = 0; j < udev.bNumInterfaces; j++) {
            rc = usbip_net_recv(sockfd, &uintf, sizeof(uintf));
            if (rc < 0) {
                err("usbip_net_recv failed: usbip_usb_intf[%d]", j);

                return -1;
            }
            usbip_net_pack_usb_interface(0, &uintf);

            usbip_names_get_class(
              class_name, sizeof(class_name), uintf.bInterfaceClass,
              uintf.bInterfaceSubClass, uintf.bInterfaceProtocol);
            printf("%11s: %2d - %s\n", "", j, class_name);
        }

        printf("\n");
    }

    return 0;
}

static int
list_exported_devices(char* host)
{
    int rc;
    int sockfd;

    sockfd = usbip_net_tcp_connect(host, usbip_port_string);
    if (sockfd < 0) {
        err("could not connect to %s:%s: %s", host, usbip_port_string,
            gai_strerror(sockfd));
        return -1;
    }
    dbg("connected to %s:%s", host, usbip_port_string);

    rc = get_exported_devices(host, sockfd);
    if (rc < 0) {
        err("failed to get device list from %s", host);
        return -1;
    }

    close(sockfd);

    return 0;
}

static void
print_device(const char* busid, const char* vendor, const char* product)
{
    printf(" - busid %s (%.4s:%.4s)\n", busid, vendor, product);
}

static void
print_product_name(char* product_name, const char* manufact,
                   const char* product_usb)
{
    printf("   %s\n", product_name);
    printf("   manufacturer: %s\n", manufact);
    printf("   product: %s\n", product_usb);
}

static int
list_devices()
{
    struct udev* udev;
    struct udev_enumerate* enumerate;
    struct udev_list_entry *devices, *dev_list_entry;
    struct udev_device* dev;
    const char* path;
    const char* idVendor;
    const char* idProduct;
    const char* bConfValue;
    const char* bNumIntfs;
    const char* busid;
    const char* manufact;
    const char* product_usb;
    char product_name[128];
    int ret = -1;

    /* Create libudev context. */
    udev = udev_new();

    /* Create libudev device enumeration. */
    enumerate = udev_enumerate_new(udev);

    /* Take only USB devices that are not hubs and do not have
     * the bInterfaceNumber attribute, i.e. are not interfaces.
     */
    udev_enumerate_add_match_subsystem(enumerate, "usb");
    udev_enumerate_add_nomatch_sysattr(enumerate, "bDeviceClass", "09");
    udev_enumerate_add_nomatch_sysattr(enumerate, "bInterfaceNumber", NULL);
    udev_enumerate_scan_devices(enumerate);

    devices = udev_enumerate_get_list_entry(enumerate);

    /* Show information about each device. */
    udev_list_entry_foreach(dev_list_entry, devices)
    {
        path = udev_list_entry_get_name(dev_list_entry);
        dev = udev_device_new_from_syspath(udev, path);

        /* Get device information. */
        idVendor = udev_device_get_sysattr_value(dev, "idVendor");
        idProduct = udev_device_get_sysattr_value(dev, "idProduct");
        bConfValue = udev_device_get_sysattr_value(dev, "bConfigurationValue");
        bNumIntfs = udev_device_get_sysattr_value(dev, "bNumInterfaces");
        busid = udev_device_get_sysname(dev);
        manufact = udev_device_get_sysattr_value(dev, "manufacturer");
        product_usb = udev_device_get_sysattr_value(dev, "product");

        if (!idVendor || !idProduct || !bConfValue || !bNumIntfs) {
            err("problem getting device attributes: %s", strerror(errno));
            goto err_out;
        }

        /* Get product name. */
        usbip_names_get_product(product_name, sizeof(product_name),
                                strtol(idVendor, NULL, 16),
                                strtol(idProduct, NULL, 16));

        /* Print information. */
        print_device(busid, idVendor, idProduct);
        print_product_name(product_name, manufact, product_usb);
        printf("\n");

        udev_device_unref(dev);
    }

    ret = 0;

err_out:
    udev_enumerate_unref(enumerate);
    udev_unref(udev);

    return ret;
}

int
usbip_list(void)
{
    int ret;

    if (usbip_names_init(USBIDS_FILE)) {
        err("failed to open %s", USBIDS_FILE);
    }

    ret = list_devices();
    usbip_names_free();

    return ret;
}