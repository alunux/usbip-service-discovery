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
#include <net/if.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "device.h"

#define NEKOFI_CAST_ADDR "225.10.10.1"
#define LISTENPORT 10296
#define MAXBUFLEN 128

static const char*
get_iface_addr(const char* iface_name)
{
    int fd;
    struct ifreq ifr;

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name, iface_name, IFNAMSIZ - 1);
    ifr.ifr_name[IFNAMSIZ - 1] = '\0';

    ioctl(fd, SIOCGIFADDR, &ifr);
    close(fd);

    return inet_ntoa(((struct sockaddr_in*)&ifr.ifr_addr)->sin_addr);
}

int
main(int argc, char* argv[])
{
    struct sockaddr_in LocalSock;
    struct ip_mreq NekoFiGroup;

    int status;
    int sockfd;
    socklen_t socklen;
    char databuf[MAXBUFLEN];
    char datarecv[MAXBUFLEN];

    GSList* usb_list = NULL;
    GSList* iterator = NULL;

    memset(&LocalSock, 0, sizeof(LocalSock));
    memset(&NekoFiGroup, 0, sizeof(NekoFiGroup));

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("can't create datagram socket");
        exit(1);
    }

    {
        int reuse = 1;
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse,
                       sizeof(reuse)) < 0) {
            perror("setting SO_REUSEADDR");
            close(sockfd);
            exit(1);
        }
    }

    LocalSock.sin_family = AF_INET;
    LocalSock.sin_port = htons(10296);
    LocalSock.sin_addr.s_addr = htonl(INADDR_ANY);

    status = bind(sockfd, (struct sockaddr*)&LocalSock, sizeof(LocalSock));
    if (status < 0) {
        perror("binding datagram socket");
        close(sockfd);
        exit(1);
    }

    NekoFiGroup.imr_multiaddr.s_addr = inet_addr(NEKOFI_CAST_ADDR);
    NekoFiGroup.imr_interface.s_addr = inet_addr(get_iface_addr("ens3"));

    if (setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&NekoFiGroup,
                   sizeof(NekoFiGroup)) < 0) {
        perror("setup multicast group");
        close(sockfd);
        exit(1);
    }

    while (1) {
        usb_list = usb_devices_list();
        for (iterator = usb_list; iterator != NULL; iterator = iterator->next) {
            snprintf(databuf, sizeof(databuf), "%s",
                     ((UsbDevice*)iterator->data)->manufact);
        }

        finish_dev_usage(usb_list);
        g_slist_free(usb_list);

        printf("NekoFi server: listening on %s...\n", get_iface_addr("ens3"));
        socklen = sizeof(LocalSock);
        status = recvfrom(sockfd, datarecv, MAXBUFLEN, 0,
                          (struct sockaddr*)&LocalSock, &socklen);

        if (status < 0) {
            perror("recvfrom");
        } else {
            printf("received %d bytes from %s\n", status,
                   inet_ntoa(LocalSock.sin_addr));
            printf("NekoFi server: packet is %d bytes\n", status);
            databuf[status] = '\0';
            printf("NekoFi server: packet contains \"%s\"\n", datarecv);
            status = sendto(sockfd, databuf, strlen(databuf), 0,
                            (struct sockaddr*)&LocalSock, socklen);
        }
        printf("\n");
    }

    shutdown(sockfd, SHUT_RDWR);
    close(sockfd);

    return EXIT_SUCCESS;
}
