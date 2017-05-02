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
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define NEKOFI_CAST_ADDR "225.10.10.1"
#define LISTENPORT 10296
#define HW_IFACE_NAME "virbr0"

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
    struct in_addr LocalIface;
    struct sockaddr_in NekoFiGroupSock;

    struct timeval time_val;
    time_val.tv_sec = 1;
    time_val.tv_usec = 0;

    int status;
    int sockfd;
    int ack = 1;
    socklen_t socklen;

    pid_t pid;

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

    LocalIface.s_addr = inet_addr(get_iface_addr(HW_IFACE_NAME));
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
    status = sendto(sockfd, &ack, sizeof(ack), 0,
                    (struct sockaddr*)&NekoFiGroupSock, socklen);

    int n_node = 0;
    while (1) {
        status = recvfrom(sockfd, &ack, sizeof(ack), 0,
                          (struct sockaddr*)&NekoFiGroupSock, &socklen);

        if (status < 0) {
            close(sockfd);
            break;
        }

        pid = fork();
        if (pid == 0) {
            printf("received %d bytes from %s\n", status,
                   inet_ntoa(NekoFiGroupSock.sin_addr));
            printf("ack from server = %d\n", ack);
            close(sockfd);
            exit(0);
        }
        n_node++;
    }

    printf("Total NekoFi node: %d\n", n_node);

    return EXIT_SUCCESS;
}
