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

#include "detect_iface.h"
#include "discover.h"
#include "multicast_event.h"

#define USBIP_GROUP_ADDR "239.255.0.1"
#define JSON_PORT 10796
#define LISTENPORT 10296

static int discover_recv_connect(const char *node_addr)
{
    struct sockaddr_in serv_addr;
    int sockfd;

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(JSON_PORT);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("recv_data: socket");
        return -1;
    }

    if (inet_pton(AF_INET, node_addr, &serv_addr.sin_addr) <= 0) {
        perror("recv_data: inet_pton");
        return -1;
    }

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("recv_data: connect");
        return -1;
    }

    return sockfd;
}

static json_object *discover_recv_usb_desc_json(const char node_addr[])
{
    struct timespec start, stop;

    static json_object *usb_json;

    int sockfd, n;
    uint32_t json_size;

    sockfd = discover_recv_connect(node_addr);
    if (sockfd < 0) {
        perror("discover_recv_connect");
        return NULL;
    }

    n = recv(sockfd, &json_size, sizeof(json_size), 0);
    if (n < 0) {
        perror("recv_data: json_size");
        return NULL;
    }

    char *recvBuff = calloc(1, json_size * sizeof(char));
    do {
        n = recv(sockfd, recvBuff, json_size, 0);
        if (n < 0) {
            perror("recv_data: usb_json");
            return NULL;
        }
    } while ((uint32_t)(n) != json_size);

    usb_json = json_tokener_parse(recvBuff);

    free(recvBuff);
    close(sockfd);
    return usb_json;
}

const char *discover_query_usb_desc(json_object *root, const char *key)
{
    json_object *ret_val;

    if (json_object_object_get_ex(root, key, &ret_val))
        return json_object_get_string(ret_val);

    return NULL;
}

json_object *discover_get_json(void)
{
    struct timespec start, stop;

    struct in_addr LocalIface;
    struct sockaddr_in NekoFiGroupSock;
    json_object *usb_json;

    int status;
    int sockfd;
    int ack = 1;
    int n_node = 0;
    char node_addr[10][16];
    const char *iface_name;
    socklen_t socklen;

    pid_t pid;
    int fd[10][2];
    ssize_t ret_wr;

    memset(&NekoFiGroupSock, 0, sizeof(NekoFiGroupSock));
    usb_json = json_object_new_object();

    iface_name = get_iface_addr();
    if (iface_name == NULL) {
        printf("Can't find wireless interface\n");
        goto complete;
    }

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("failed to create socket");
        goto complete;
    }

    NekoFiGroupSock.sin_family = AF_INET;
    NekoFiGroupSock.sin_port = htons(LISTENPORT);
    NekoFiGroupSock.sin_addr.s_addr = inet_addr(USBIP_GROUP_ADDR);
    LocalIface.s_addr = inet_addr(iface_name);

    multicast_set_ip_iface(sockfd, &LocalIface);
    multicast_set_socket_timeout(sockfd, 1, 0);

    socklen = sizeof(NekoFiGroupSock);
    status = sendto(sockfd, &ack, sizeof(ack), 0, (struct sockaddr *)&NekoFiGroupSock, socklen);
    if (status < 0) {
        perror("failed to sendto");
        goto complete;
    }

    // printf("Uji kinerja pencarian perangkat penyedia\n");
    while (1) {
        clock_gettime(CLOCK_REALTIME, &start);
        status = recvfrom(sockfd, NULL, 0, 0, (struct sockaddr *)&NekoFiGroupSock, &socklen);
        clock_gettime(CLOCK_REALTIME, &stop);

        if (status < 0) {
            close(sockfd);
            break;
        }

        double result = (stop.tv_sec - start.tv_sec) * 1e3 +
                        (stop.tv_nsec - start.tv_nsec) / 1e6; // in milidetik
        // printf(
        //   "%s: %f milidetik\n", inet_ntoa(NekoFiGroupSock.sin_addr), result);

        if (pipe(fd[n_node]) != 0) {
            printf("Could not create new pipe %d", n_node);
            continue;
        }

        pid = fork();
        if (pid == 0) {
            strncpy(node_addr[n_node], inet_ntoa(NekoFiGroupSock.sin_addr),
                    sizeof(node_addr[n_node]));
            close(fd[n_node][0]);

            do {
                ret_wr = write(fd[n_node][1], node_addr[n_node], sizeof(node_addr[n_node]));
            } while (ret_wr != sizeof(node_addr[n_node]));

            close(fd[n_node][1]);
            close(sockfd);
            exit(0);
        }

        if (pid != 0) {
            close(fd[n_node][1]);
            wait(NULL);

            do {
                ret_wr = read(fd[n_node][0], node_addr[n_node], sizeof(node_addr[n_node]));
            } while (ret_wr != sizeof(node_addr[n_node]));

            close(fd[n_node][0]);
        }

        n_node++;
    }

    clock_gettime(CLOCK_REALTIME, &start);
    json_object *add_usb_tolist;
    if (n_node > 0) {
        for (int i = 0; i < n_node; i++) {
            add_usb_tolist = discover_recv_usb_desc_json(node_addr[i]);
            if (add_usb_tolist == NULL)
                continue;

            json_object_object_add(usb_json, node_addr[i], add_usb_tolist);
        }

        // g_print("%s\n",
        //        json_object_to_json_string_ext(
        //          usb_json, JSON_C_TO_STRING_SPACED |
        //          JSON_C_TO_STRING_PRETTY));
    }
    clock_gettime(CLOCK_REALTIME, &stop);
    double result =
        (stop.tv_sec - start.tv_sec) * 1e3 + (stop.tv_nsec - start.tv_nsec) / 1e6; // in milidetik
    g_print("%f milidetik\n", result);

complete:
    return usb_json;
}
