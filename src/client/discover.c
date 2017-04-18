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
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define NEKOFI_CAST_ADDR "225.10.10.1"
#define LISTENPORT 10296
#define MAXBUFLEN 128

int
main(int argc, char* argv[])
{
    struct in_addr LocalIface;
    struct sockaddr_in NekoFiGroupSock;

    int status;
    int sockfd;
    int socklen;
    char databuf[MAXBUFLEN];

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

    LocalIface.s_addr = inet_addr("192.168.122.1");
    if (setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_IF, (char*)&LocalIface,
                   sizeof(LocalIface)) < 0) {
        perror("setting local interface");
        exit(1);
    }

    strcpy(databuf, "Halo NekoFi");

    socklen = sizeof(NekoFiGroupSock);
    status = sendto(sockfd, databuf, strlen(databuf), 0,
                    (struct sockaddr*)&NekoFiGroupSock, socklen);

    printf("databuf = %s\n", databuf);
    printf("sent %d bytes to %s\n", status,
           inet_ntoa(NekoFiGroupSock.sin_addr));
    shutdown(sockfd, SHUT_RDWR);
    close(sockfd);

    return EXIT_SUCCESS;
}
