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

#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "detect_iface.h"
#include "multicast_event.h"

#define NEKOFI_CAST_ADDR "225.10.10.1"
#define LISTENPORT 10297

int
multicast_set_ip_reuse(int sockfd)
{
    char reuse = '0';
    int ret = 0;

    ret = setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_LOOP, (char*)&reuse,
                     sizeof(reuse));
    if (ret < 0) {
        perror("setting IP_MULTICAST_LOOP");
        close(sockfd);
    }

    return ret;
}

int
multicast_set_ip_iface(int sockfd, struct in_addr* LocalIface)
{
    int ret = 0;

    ret = setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_IF, (char*)LocalIface,
                     sizeof(struct in_addr));
    if (ret < 0) {
        perror("setting local interface");
    }

    return ret;
}

int
multicast_set_socket_timeout(int sockfd, time_t sec, suseconds_t usec)
{
    struct timeval sock_timeout;
    sock_timeout.tv_sec = sec;
    sock_timeout.tv_usec = usec;

    int ret = 0;

    ret = setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO,
                     (const char*)&sock_timeout, sizeof(struct timeval));
    if (ret < 0) {
        perror("setting socket timeout");
    }

    return ret;
}

int
announce_client_event(void)
{
    struct in_addr LocalIface;
    struct sockaddr_in NekoFiGroupSock;

    int sockfd;
    char ack[] = "1";
    const char* iface_name;
    socklen_t socklen;

    memset(&NekoFiGroupSock, 0, sizeof(NekoFiGroupSock));

    iface_name = get_iface_addr();
    if (iface_name == NULL) {
        printf("Can't find wireless interface\n");
        return -1;
    }

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("failed to create socket");
        return -1;
    }

    NekoFiGroupSock.sin_family = AF_INET;
    NekoFiGroupSock.sin_port = htons(LISTENPORT);
    NekoFiGroupSock.sin_addr.s_addr = inet_addr(NEKOFI_CAST_ADDR);
    LocalIface.s_addr = inet_addr(iface_name);

    multicast_set_ip_iface(sockfd, &LocalIface);
    multicast_set_socket_timeout(sockfd, 1, 0);

    socklen = sizeof(NekoFiGroupSock);
    sendto(sockfd, &ack, strlen(ack), 0, (struct sockaddr*)&NekoFiGroupSock,
           socklen);
    close(sockfd);

    return 0;
}
