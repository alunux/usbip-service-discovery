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

#define NEKOFI_CAST_ADDR "225.10.10.1"
#define JSON_PORT 10796
#define LISTENPORT 10296
#define HW_IFACE_NAME "virbr0"

/*
    TODO:
    This function must return json_object or JSON string
    to parent proccess and store the value to linked list data
    structure. The data will be used by NekoFi GUI proccess.
*/
static int
recv_usb_list_json(char node_addr[], size_t json_size)
{
    int sockfd = 0, n = 0;
    char* recvBuff = (char*)malloc(json_size + 1);
    struct sockaddr_in serv_addr;

    json_object* usb_json;

    memset(recvBuff, '0', json_size + 1);
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

    n = recv(sockfd, recvBuff, json_size + 1, 0);
    if (n < 0) {
        perror("recv_data: recv");
        exit(1);
    }

    recvBuff[n] = '\0';

    usb_json = json_tokener_parse(recvBuff);

    /* debugging purpose */
    printf("%s\n",
           json_object_to_json_string_ext(usb_json, JSON_C_TO_STRING_SPACED |
                                                      JSON_C_TO_STRING_PRETTY));

    json_object_object_foreach(usb_json, key, val)
    {
        if (json_object_get_type(val) == json_type_object) {
            json_object_object_foreach(val, key1, val1)
            {
                printf("%s = %s\n", key1, json_object_get_string(val1));
            }
        }
        printf("\n");
    }

    free(recvBuff);
    json_object_put(usb_json);
    close(sockfd);

    return 0;
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
    struct in_addr LocalIface;
    struct sockaddr_in NekoFiGroupSock;

    struct timeval time_val;
    time_val.tv_sec = 0;
    time_val.tv_usec = 100000;

    int status;
    int sockfd;
    int ack = 1;
    char* node_addr;
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
    size_t json_size;

    while (1) {
        status = recvfrom(sockfd, &json_size, sizeof(json_size), 0,
                          (struct sockaddr*)&NekoFiGroupSock, &socklen);

        if (status < 0) {
            close(sockfd);
            break;
        }

        pid = fork();
        if (pid == 0) {
            node_addr = inet_ntoa(NekoFiGroupSock.sin_addr);
            printf("FROM %s: received json_size %lu bytes\n", node_addr,
                   json_size);
            recv_usb_list_json(node_addr, json_size);
            close(sockfd);
            exit(0);
        }
        n_node++;
    }

    /* debugging purpose */
    printf("Total NekoFi node: %d\n", n_node);

    return EXIT_SUCCESS;
}
