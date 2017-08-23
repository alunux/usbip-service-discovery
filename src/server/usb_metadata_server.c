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
#include <json.h>
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

#include "device.h"

#define JSON_PORT 10796

static void
sigchld_handler(__attribute__((unused)) int s)
{
    int saved_errno = errno;
    while (waitpid(-1, NULL, WNOHANG) > 0)
        ;
    errno = saved_errno;
}

static int
sendall(int s, char* buf, uint32_t* len)
{
    uint32_t total = 0;
    uint32_t bytesleft = *len;
    int n;
    while (total < *len) {
        n = send(s, buf + total, bytesleft, 0);
        if (n == -1) {
            break;
        }
        total += n;
        bytesleft -= n;
    }
    *len = total;
    return n == -1 ? -1 : 0;
}

int
main(void)
{
    int sockfd = 0, connfd = 0, status = 0;
    struct sockaddr_in serv_addr;
    struct sigaction sa;

    json_object* usb_json = NULL;
    const char* temp_json;
    uint32_t json_size = 0;

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(JSON_PORT);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("send_data: socket");
    }

    {
        int reuse = 1;
        if (setsockopt(
              sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
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

    usb_json = get_devices();

    temp_json =
      json_object_to_json_string_ext(usb_json, JSON_C_TO_STRING_PLAIN);
    json_size = strlen(temp_json) + 1;
    char* usb_dev_json = (char*)malloc(json_size);
    memset(usb_dev_json, '\0', sizeof(char) * json_size);
    strncpy(usb_dev_json, temp_json, json_size);

    json_object_put(usb_json);

    printf("send_data: waiting for calling...\n");

    while (1) {
        connfd = accept(sockfd, (struct sockaddr*)NULL, NULL);
        if (connfd < 0) {
            perror("send_data: accept");
            continue;
        }

        if (!fork()) {
            close(sockfd);

            status = send(connfd, &json_size, sizeof(json_size), 0);
            if (status < 0) {
                perror("send_data: json_size");
            }

            status = sendall(connfd, usb_dev_json, &json_size);
            if (status < 0) {
                perror("send_data: usb_json");
            }

            close(connfd);
            exit(0);
        }
        close(connfd);
    }

    free(usb_dev_json);

    return EXIT_SUCCESS;
}