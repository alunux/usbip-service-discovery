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
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int
main(int argc, char* argv[])
{
    int sockfd = 0, n = 0;
    char recvBuff[1024];
    struct sockaddr_in serv_addr;

    memset(recvBuff, '0', sizeof(recvBuff));
    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(5000);

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("recv_data: socket");
        exit(1);
    }

    if (inet_pton(AF_INET, argv[1], &serv_addr.sin_addr) <= 0) {
        perror("recv_data: inet_pton");
        exit(1);
    }

    if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("recv_data: connect");
        exit(1);
    }

    n = recv(sockfd, recvBuff, sizeof(recvBuff) - 1, 0);
    if (n < 0) {
        perror("recv_data: recv");
        exit(1);
    }

    recvBuff[n] = '\0';
    printf("recv_data: %s\n", recvBuff);
    close(sockfd);

    return 0;
}