/*
 * Copyright (C) 2017 La Ode Muh. Fadlun Akbar <fadlun.net@gmail.com>
 *               2011 matt mooney <mfm@muteddisk.com>
 *               2005-2007 Takahiro Hirofuchi
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __USBIP_H
#define __USBIP_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

typedef struct _NekoFiDevice {
    int state;
    char* node_addr;
    char* busid;
} NekoFiDevice;

/* usbip commands */
int attach_device(char* host, char* busid);
int detach_port(char* port);
int check_device_state(char* host, char* busid);

#endif /* __USBIP_H */
