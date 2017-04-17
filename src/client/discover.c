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

#define LISTENERPORT 13000

int
main(int argc, char* argv[])
{
    int sockfd;
    int numbytes;
    int broadcast = 1;
    int ret = 0;
    struct sockaddr_in client_addr;
    struct hostent* hostname;

    if (argc != 3) {
        fprintf(stderr, "usage: client hostname message\n");
        exit(1);
    }

    hostname = gethostbyname(argv[1]);
    if (hostname == NULL) {
        perror("failed to get hostname");
        exit(1);
    }

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        perror("failed to create socket");
        exit(1);
    }

    ret = setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast,
                     sizeof(broadcast));
    if (ret == -1) {
        perror("can't setup setsockopt(SO_BROADCAST)");
        exit(1);
    }

    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(LISTENERPORT);
    client_addr.sin_addr = *((struct in_addr*)hostname->h_addr);
    memset(client_addr.sin_zero, '\0', sizeof(client_addr.sin_zero));

    numbytes = sendto(sockfd, argv[2], strlen(argv[2]), 0,
                      (struct sockaddr*)&client_addr, sizeof(client_addr));
    if (numbytes == -1) {
        perror("can't sendto the broadcast address");
        exit(1);
    }

    printf("sent %d bytes to %s\n", numbytes, inet_ntoa(client_addr.sin_addr));
    close(sockfd);

    return EXIT_SUCCESS;
}
