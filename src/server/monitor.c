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

#include <arpa/inet.h>
#include <json.h>
#include <libudev.h>
#include <net/if.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "detect_iface.h"

#define NEKOFI_CAST_ADDR "225.10.10.1"
#define LISTENPORT 10297

static void
broadcast_event(void)
{
    struct in_addr LocalIface;
    struct sockaddr_in NekoFiGroupSock;

    struct timeval time_val;
    time_val.tv_sec = 1;
    time_val.tv_usec = 0;

    int sockfd;
    char ack[] = "1";
    socklen_t socklen;

    memset(&NekoFiGroupSock, 0, sizeof(NekoFiGroupSock));

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("failed to create socket");
        exit(1);
    }

    NekoFiGroupSock.sin_family = AF_INET;
    NekoFiGroupSock.sin_port = htons(LISTENPORT);
    NekoFiGroupSock.sin_addr.s_addr = inet_addr(NEKOFI_CAST_ADDR);

    {
        char reuse = '0';
        if (setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_LOOP, (char*)&reuse,
                       sizeof(reuse)) < 0) {
            perror("setting IP_MULTICAST_LOOP");
            close(sockfd);
            exit(1);
        }
    }

    LocalIface.s_addr = inet_addr(get_iface_addr());
    if (setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_IF, (char*)&LocalIface,
                   sizeof(LocalIface)) < 0) {
        perror("setting local interface");
        exit(1);
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&time_val,
                   sizeof(struct timeval)) < 0) {
        perror("setting socket timeout");
        exit(1);
    }

    socklen = sizeof(NekoFiGroupSock);
    sendto(sockfd, &ack, strlen(ack), 0, (struct sockaddr*)&NekoFiGroupSock,
           socklen);

    close(sockfd);
}

int
main(void)
{
    struct udev* udev;
    struct udev_device* dev;
    struct udev_monitor* mon;

    int fd;

    udev = udev_new();
    if (udev == NULL) {
        perror("udev");
        exit(1);
    }

    mon = udev_monitor_new_from_netlink(udev, "udev");
    udev_monitor_filter_add_match_subsystem_devtype(mon, "usb", NULL);
    udev_monitor_enable_receiving(mon);
    fd = udev_monitor_get_fd(mon);

    broadcast_event();

    while (1) {
        fd_set fds;
        struct timeval tv;
        int ret = 0;

        const char* path;

        FD_ZERO(&fds);
        FD_SET(fd, &fds);
        tv.tv_sec = 0;
        tv.tv_usec = 0;

        ret = select(fd + 1, &fds, NULL, NULL, &tv);
        if (ret > 0 && FD_ISSET(fd, &fds)) {
            dev = udev_monitor_receive_device(mon);
            if (dev) {
                path = udev_device_get_devnode(dev);
                if (path != NULL) {
                    if (system("killall send_data") == 0) {
                        system("./send_data &");
                        broadcast_event();
                    }
                }
                udev_device_unref(dev);
            }
        }

        usleep(250 * 1000);
    }

    return EXIT_SUCCESS;
}