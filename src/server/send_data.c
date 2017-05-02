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
#include <sys/wait.h>
#include <unistd.h>

static void
sigchld_handler(int s)
{
    int saved_errno = errno;
    while (waitpid(-1, NULL, WNOHANG) > 0)
        ;
    errno = saved_errno;
}

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
    int sockfd = 0, connfd = 0, status = 0;
    struct sockaddr_in serv_addr;
    struct sigaction sa;

    char sendBuff[1025];

    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(5000);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("send_data: socket");
    }

    {
        int reuse = 1;
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse,
                       sizeof(reuse)) < 0) {
            perror("send_data: setsockopt");
            close(sockfd);
            exit(1);
        }
    }

    if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("send_data: bind");
        close(sockfd);
        exit(1);
    }

    if (listen(sockfd, 10) < 0) {
        perror("send_data: listen");
        exit(1);
    }

    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    if (sigaction(SIGCHLD, &sa, NULL) < 0) {
        perror("sigaction");
        exit(1);
    }

    const char* usb_dev_json = "JSON Data";
    printf("send_data: waiting for calling...\n");

    while (1) {
        connfd = accept(sockfd, (struct sockaddr*)NULL, NULL);
        if (connfd < 0) {
            perror("send_data: accept");
            continue;
        }

        if (!fork()) {
            close(sockfd);

            status = send(connfd, usb_dev_json, strlen(usb_dev_json), 0);
            if (status < 0) {
                perror("send_data: send");
            }

            close(connfd);
            exit(0);
        }
        close(connfd);
    }
    return EXIT_SUCCESS;
}