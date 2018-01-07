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

#include "usb_desc.h"

struct _UsbDesc {
    GObject parent;

    gchar* name;
    gchar* idvendor;
    gchar* idproduct;
    gchar* manufacturer;
    gchar* busid;
    gchar* node_addr;
    gboolean state;
};

G_DEFINE_TYPE(UsbDesc, usb_desc, G_TYPE_OBJECT);

enum {
    PROP_0,
    PROP_NAME,
    PROP_IDVENDOR,
    PROP_IDPRODUCT,
    PROP_MANUFACTURER,
    PROP_BUSID,
    PROP_NODE,
    PROP_STATE,
    LAST_PROP
};

static GParamSpec* properties[LAST_PROP];

UsbDesc*
usb_desc_new(void)
{
    return g_object_new(USB_TYPE_DESC, NULL);
}

static void
usb_desc_init(__attribute__((unused)) UsbDesc* self)
{
}

static void
usb_desc_finalize(GObject* obj)
{
    UsbDesc* self = USB_DESC(obj);

    g_free(self->name);
    g_free(self->idvendor);
    g_free(self->idproduct);
    g_free(self->manufacturer);
    g_free(self->busid);
    g_free(self->node_addr);

    G_OBJECT_CLASS(usb_desc_parent_class)->finalize(obj);
}

static void
usb_desc_get_property(GObject* obj,
                      guint prop_id,
                      GValue* value,
                      GParamSpec* pspec)
{
    UsbDesc* self = USB_DESC(obj);

    switch (prop_id) {
        case PROP_NAME:
            g_value_set_string(value, usb_desc_get_name(self));
            break;
        case PROP_IDVENDOR:
            g_value_set_string(value, usb_desc_get_idvendor(self));
            break;
        case PROP_IDPRODUCT:
            g_value_set_string(value, usb_desc_get_idproduct(self));
            break;
        case PROP_MANUFACTURER:
            g_value_set_string(value, usb_desc_get_manufacturer(self));
            break;
        case PROP_BUSID:
            g_value_set_string(value, usb_desc_get_busid(self));
            break;
        case PROP_NODE:
            g_value_set_string(value, usb_desc_get_node_addr(self));
            break;
        case PROP_STATE:
            g_value_set_boolean(value, usb_desc_get_state(self));
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, prop_id, pspec);
    }
}

static void
usb_desc_set_property(GObject* obj,
                      guint prop_id,
                      const GValue* value,
                      GParamSpec* pspec)
{
    UsbDesc* self = USB_DESC(obj);

    switch (prop_id) {
        case PROP_NAME:
            usb_desc_set_name(self, g_value_get_string(value));
            break;
        case PROP_IDVENDOR:
            usb_desc_set_idvendor(self, g_value_get_string(value));
            break;
        case PROP_IDPRODUCT:
            usb_desc_set_idproduct(self, g_value_get_string(value));
            break;
        case PROP_MANUFACTURER:
            usb_desc_set_manufacturer(self, g_value_get_string(value));
            break;
        case PROP_BUSID:
            usb_desc_set_busid(self, g_value_get_string(value));
            break;
        case PROP_NODE:
            usb_desc_set_node_addr(self, g_value_get_string(value));
            break;
        case PROP_STATE:
            usb_desc_set_state(self, g_value_get_boolean(value));
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, prop_id, pspec);
    }
}

static void
usb_desc_class_init(UsbDescClass* klass)
{
    GObjectClass* obj_class = G_OBJECT_CLASS(klass);

    obj_class->finalize = usb_desc_finalize;
    obj_class->get_property = usb_desc_get_property;
    obj_class->set_property = usb_desc_set_property;

    properties[PROP_NAME] =
      g_param_spec_string("name",
                          "Name",
                          "The name of the USB device",
                          NULL,
                          (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

    properties[PROP_IDVENDOR] =
      g_param_spec_string("id-vendor",
                          "idVendor",
                          "The Vendor ID of the USB device",
                          NULL,
                          (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

    properties[PROP_IDPRODUCT] =
      g_param_spec_string("id-product",
                          "idProduct",
                          "The Product ID of the USB device",
                          NULL,
                          (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

    properties[PROP_MANUFACTURER] =
      g_param_spec_string("manufacturer",
                          "Manufacturer",
                          "The manufacturer of the USB device",
                          NULL,
                          (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

    properties[PROP_BUSID] =
      g_param_spec_string("busid",
                          "Busid",
                          "The busid of the USB device",
                          NULL,
                          (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

    properties[PROP_NODE] =
      g_param_spec_string("node-addr",
                          "NodeAddr",
                          "The IP address of the USB device",
                          NULL,
                          (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

    properties[PROP_STATE] =
      g_param_spec_boolean("state",
                           "State",
                           "The state (attachable) of the USB device",
                           TRUE,
                           (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

    g_object_class_install_properties(obj_class, LAST_PROP, properties);
}

void
usb_desc_print(UsbDesc *self)
{
    g_return_if_fail(USB_IS_DESC(self));
    g_print("Name: %s\n"
            "idVendor: %s\n"
            "idProduct: %s\n"
            "Manufacturer: %s\n"
            "BUSID: %s\n"
            "Node: %s\n"
            "State: %s\n\n",
            self->name,
            self->idvendor,
            self->idproduct,
            self->manufacturer,
            self->busid,
            self->node_addr,
            (self->state == TRUE) ? "Attachable" : "Detachable");
}

const gchar*
usb_desc_get_name(UsbDesc* self)
{
    g_return_val_if_fail(USB_IS_DESC(self), NULL);
    return self->name;
}

const gchar*
usb_desc_get_idvendor(UsbDesc* self)
{
    g_return_val_if_fail(USB_IS_DESC(self), NULL);
    return self->idvendor;
}

const gchar*
usb_desc_get_idproduct(UsbDesc* self)
{
    g_return_val_if_fail(USB_IS_DESC(self), NULL);
    return self->idproduct;
}

const gchar*
usb_desc_get_manufacturer(UsbDesc* self)
{
    g_return_val_if_fail(USB_IS_DESC(self), NULL);
    return self->manufacturer;
}

const gchar*
usb_desc_get_busid(UsbDesc* self)
{
    g_return_val_if_fail(USB_IS_DESC(self), NULL);
    return self->busid;
}

const gchar*
usb_desc_get_node_addr(UsbDesc* self)
{
    g_return_val_if_fail(USB_IS_DESC(self), NULL);
    return self->node_addr;
}

gboolean
usb_desc_get_state(UsbDesc* self)
{
    g_return_val_if_fail(USB_IS_DESC(self), FALSE);
    return self->state;
}

void
usb_desc_set_name(UsbDesc* self, const gchar* name)
{
    g_return_if_fail(USB_IS_DESC(self));
    g_free(self->name);
    self->name = g_strdup(name);
    g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_NAME]);
}

void
usb_desc_set_idvendor(UsbDesc* self, const gchar* idvendor)
{
    g_return_if_fail(USB_IS_DESC(self));
    g_free(self->idvendor);
    self->idvendor = g_strdup(idvendor);
    g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_IDVENDOR]);
}

void
usb_desc_set_idproduct(UsbDesc* self, const gchar* idproduct)
{
    g_return_if_fail(USB_IS_DESC(self));
    g_free(self->idproduct);
    self->idproduct = g_strdup(idproduct);
    g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_IDPRODUCT]);
}

void
usb_desc_set_manufacturer(UsbDesc* self, const gchar* manufacturer)
{
    g_return_if_fail(USB_IS_DESC(self));
    g_free(self->manufacturer);
    self->manufacturer = g_strdup(manufacturer);
    g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_MANUFACTURER]);
}

void
usb_desc_set_busid(UsbDesc* self, const gchar* busid)
{
    g_return_if_fail(USB_IS_DESC(self));
    g_free(self->busid);
    self->busid = g_strdup(busid);
    g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_BUSID]);
}

void
usb_desc_set_node_addr(UsbDesc* self, const gchar* node_addr)
{
    g_return_if_fail(USB_IS_DESC(self));
    g_free(self->node_addr);
    self->node_addr = g_strdup(node_addr);
    g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_NODE]);
}

void
usb_desc_set_state(UsbDesc* self, gboolean attachable)
{
    g_return_if_fail(USB_IS_DESC(self));
    self->state = attachable;
    g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_STATE]);
}