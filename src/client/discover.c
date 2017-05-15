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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "discover.h"

#define NEKOFI_CAST_ADDR "225.10.10.1"
#define JSON_PORT 10796
#define LISTENPORT 10296
#define HW_IFACE_NAME "virbr0"

static json_object*
recv_usb_list_json(char node_addr[])
{
    int sockfd, n;
    struct sockaddr_in serv_addr;

    static json_object* usb_json;
    uint32_t json_size;

    // char recvBuff[4096];

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(JSON_PORT);

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("recv_data: socket");
        exit(1);
    }

    if (inet_pton(AF_INET, node_addr, &serv_addr.sin_addr) <= 0) {
        perror("recv_data: inet_pton");
        exit(1);
    }

    if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("recv_data: connect");
        exit(1);
    }

    n = recv(sockfd, &json_size, sizeof(json_size), 0);
    if (n < 0) {
        perror("recv_data: json_size");
        exit(1);
    }

    char* recvBuff = calloc(1, json_size * sizeof(char));

    do {
        n = recv(sockfd, recvBuff, json_size, 0);
        if (n < 0) {
            perror("recv_data: usb_json");
            exit(1);
        }
    } while (n != json_size);

    /* debugging purpose */
    printf("FROM: %s, json_size: %u, n: %d\n", node_addr, json_size, n);

    usb_json = json_tokener_parse(recvBuff);

    free(recvBuff);
    close(sockfd);
    return usb_json;
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

char*
get_usb_desc(json_object* root, const char* key)
{
    json_object* ret_val;

    if (json_object_object_get_ex(root, key, &ret_val)) {
        return json_object_get_string(ret_val);
    }

    return NULL;
}

json_object*
nekofi_discover_json(void)
{
    struct in_addr LocalIface;
    struct sockaddr_in NekoFiGroupSock;

    struct timeval time_val;
    time_val.tv_sec = 1;
    time_val.tv_usec = 0;

    int status;
    int sockfd;
    int ack = 1;
    char node_addr[10][16];
    socklen_t socklen;

    pid_t pid;
    int fd[10][2];

    json_object* usb_json;

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
        status = recvfrom(sockfd, NULL, 0, 0,
                          (struct sockaddr*)&NekoFiGroupSock, &socklen);

        if (status < 0) {
            close(sockfd);
            break;
        }

        if (pipe(fd[n_node]) != 0) {
            printf("Could not create new pipe %d", n_node);
            exit(1);
        }

        pid = fork();
        if (pid == 0) {
            strncpy(node_addr[n_node], inet_ntoa(NekoFiGroupSock.sin_addr),
                    sizeof(node_addr[n_node]));
            printf("NODE FOUND: %s\n", node_addr[n_node]);
            close(fd[n_node][0]);
            write(fd[n_node][1], node_addr[n_node], sizeof(node_addr[n_node]));
            close(fd[n_node][1]);
            close(sockfd);
            exit(0);
        }

        if (pid != 0) {
            close(fd[n_node][1]);
            wait(NULL);
            read(fd[n_node][0], node_addr[n_node], sizeof(node_addr[n_node]));
            close(fd[n_node][0]);
        }

        n_node++;
    }

    usb_json = json_object_new_object();
    /* debugging purpose */
    json_object* add_usb_tolist;
    printf("Total NekoFi node: %d\n", n_node);
    if (n_node > 0) {
        for (int i = 0; i < n_node; i++) {
            add_usb_tolist = recv_usb_list_json(node_addr[i]);
            json_object_object_add(usb_json, node_addr[i], add_usb_tolist);
        }

        printf("%s\n",
               json_object_to_json_string_ext(
                 usb_json, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY));
    }

    return usb_json;
}
