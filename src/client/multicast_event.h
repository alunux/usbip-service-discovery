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

#ifndef MULTICAST_EVENT_H
#define MULTICAST_EVENT_H

#include <arpa/inet.h>

int multicast_set_ip_reuse(int sockfd);
int multicast_set_ip_iface(int sockfd, struct in_addr *LocalIface);
int multicast_set_socket_timeout(int sockfd, time_t sec, suseconds_t usec);
int announce_client_event(void);

#endif /* MULTICAST_EVENT_H */