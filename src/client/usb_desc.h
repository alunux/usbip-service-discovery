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

#ifndef _USB_DESC_H
#define _USB_DESC_H

#include <glib-object.h>

#define USB_TYPE_DESC (usb_desc_get_type())
G_DECLARE_FINAL_TYPE(UsbDesc, usb_desc, USB, DESC, GObject)

UsbDesc*
usb_desc_new(void);

void
usb_desc_print(UsbDesc *self);
const gchar*
usb_desc_get_name(UsbDesc* self);
const gchar*
usb_desc_get_idvendor(UsbDesc* self);
const gchar*
usb_desc_get_idproduct(UsbDesc* self);
const gchar*
usb_desc_get_manufacturer(UsbDesc* self);
const gchar*
usb_desc_get_busid(UsbDesc* self);
const gchar*
usb_desc_get_node_addr(UsbDesc* self);
gboolean
usb_desc_get_state(UsbDesc* self);

void
usb_desc_set_name(UsbDesc* self, const gchar* name);
void
usb_desc_set_idvendor(UsbDesc* self, const gchar* idvendor);
void
usb_desc_set_idproduct(UsbDesc* self, const gchar* idproduct);
void
usb_desc_set_manufacturer(UsbDesc* self, const gchar* manufacturer);
void
usb_desc_set_busid(UsbDesc* self, const gchar* busid);
void
usb_desc_set_node_addr(UsbDesc* self, const gchar* node_addr);
void
usb_desc_set_state(UsbDesc* self, gboolean attachable);

#endif /* _USB_DESC_H */