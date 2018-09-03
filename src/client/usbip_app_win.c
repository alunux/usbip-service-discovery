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

#include <gtk/gtk.h>
#include <json-c/json.h>
#include <netdb.h>

#include "discover.h"
#include "usb_desc.h"
#include "usbip.h"
#include "usbip_app.h"
#include "usbip_app_win.h"

struct _UsbipAppWin {
    GtkApplicationWindow parent;
};

typedef struct _UsbipAppWinPrivate UsbipAppWinPrivate;

struct _UsbipAppWinPrivate {
    GtkWidget *win_scroll;
    GtkWidget *list_dev;
    GtkWidget *button_add_dev;
    GList *devs;
    GList *devs_attached;
};

G_DEFINE_TYPE_WITH_PRIVATE(UsbipAppWin, usbip_app_win, GTK_TYPE_APPLICATION_WINDOW)

static GtkWidget *usbip_app_win_empty(void)
{
    g_autofree gchar *no_mess = NULL;
    no_mess = g_strdup("<span size=\"x-large\">There are no USB devices "
                       "found\nin your area . . .</span>");

    GtkWidget *label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(label), no_mess);
    gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_CENTER);
    gtk_widget_show(label);

    return label;
}

static void usbip_app_win_init(UsbipAppWin *app)
{
    UsbipAppWin *win = USBIP_APP_WIN(gtk_widget_get_toplevel(GTK_WIDGET(app)));
    UsbipAppWinPrivate *self = usbip_app_win_get_instance_private(win);

    self->devs = NULL;
    gtk_widget_init_template(GTK_WIDGET(app));
    gtk_list_box_set_placeholder(GTK_LIST_BOX(self->list_dev), usbip_app_win_empty());
}

static void usbip_device_control_done(GObject *button, GAsyncResult *state, gpointer data)
{
    g_return_if_fail(g_task_is_valid(state, button));

    UsbDesc *dev = data;

    if (g_task_propagate_boolean(G_TASK(state), NULL)) {
        usb_desc_print(dev);
    } else {
        g_print("Failed to %s\n"
                "\t-> %s\n"
                "\t-> %s\n"
                "\t-> %s\n",
                (usb_desc_get_state(dev) == TRUE) ? "Attach" : "Detach", usb_desc_get_name(dev),
                usb_desc_get_node_addr(dev), usb_desc_get_busid(dev));
    }
}

static void usbip_device_control(GTask *task, gpointer obj, gpointer data,
                                 __attribute__((unused)) GCancellable *cancel)
{
    GtkWidget *usb_state = obj;
    UsbDesc *dev = data;
    const gchar *host = usb_desc_get_node_addr(dev);
    const gchar *busid = usb_desc_get_busid(dev);
    gboolean ret = TRUE;

    if (!g_strcmp0(gtk_button_get_label(GTK_BUTTON(usb_state)), "Attach")) {
        int port_n = attach_device(host, busid);
        gchar *port_s = g_strdup_printf("%d", port_n);
        if (port_n < 0) {
            ret = FALSE;
        } else {
            gtk_button_set_label(GTK_BUTTON(usb_state), "Detach");
            usb_desc_set_state(dev, FALSE);
            usb_desc_set_port(dev, port_s);
        }
        g_free(port_s);
    } else {
        if (detach_port(usb_desc_get_port(dev)) < 0) {
            ret = FALSE;
        } else {
            gtk_button_set_label(GTK_BUTTON(usb_state), "Attach");
            usb_desc_set_state(dev, TRUE);
            usb_desc_set_port(dev, NULL);
        }
    }

    g_task_return_boolean(task, ret);
}

static void usbip_state_changed(GtkWidget *usb_state, UsbDesc *dev)
{
    GTask *task = g_task_new(usb_state, NULL, usbip_device_control_done, dev);

    g_task_set_task_data(task, g_object_ref(dev), g_object_unref);
    g_task_run_in_thread(task, usbip_device_control);
    g_object_unref(task);
}

static GtkWidget *create_usbip_entry(UsbDesc *dev)
{
    g_autofree gchar *devs_desc = g_strdup_printf(
        "<b>%s</b>\nidVendor: %s\nidProduct: %s\nManufacturer: "
        "%s\nBUSID: %s\nNode: %s\n",
        usb_desc_get_name(dev), usb_desc_get_idvendor(dev), usb_desc_get_idproduct(dev),
        usb_desc_get_manufacturer(dev), usb_desc_get_busid(dev), usb_desc_get_node_addr(dev));

#define WIDGET_FROM_BUILDER(x, y) (GTK_WIDGET(gtk_builder_get_object(x, y)))

    GtkWidget *usb_entry_row = gtk_list_box_row_new();
    gtk_list_box_row_set_activatable(GTK_LIST_BOX_ROW(usb_entry_row), FALSE);
    GtkBuilder *usb_entry_builder =
        gtk_builder_new_from_resource("/org/alunux/usbipapp/usbip-entry.ui");

    GtkWidget *usb_info = WIDGET_FROM_BUILDER(usb_entry_builder, "label");
    gtk_label_set_markup(GTK_LABEL(usb_info), devs_desc);

    GtkWidget *usb_state = WIDGET_FROM_BUILDER(usb_entry_builder, "button");
    gtk_button_set_label(GTK_BUTTON(usb_state),
                         (usb_desc_get_state(dev) == TRUE) ? "Attach" : "Detach");
    g_signal_connect(usb_state, "clicked", G_CALLBACK(usbip_state_changed), dev);

    GtkWidget *usb_entry_box = WIDGET_FROM_BUILDER(usb_entry_builder, "box");
    gtk_container_add(GTK_CONTAINER(usb_entry_row), usb_entry_box);

    gtk_widget_show_all(usb_entry_row);
    g_clear_object(&usb_entry_builder);

#undef WIDGET_FROM_BUILDER

    return usb_entry_row;
}

static const gchar *query_usb_id(json_object *root, const gchar *key)
{
    json_object *ret_val;

    if (json_object_object_get_ex(root, key, &ret_val))
        return json_object_get_string(ret_val);

    return NULL;
}

static void clear_list_dev(GtkWidget *row, __attribute__((unused)) gpointer data)
{
    gtk_widget_destroy(row);
}

static void refresh_list_thread(GTask *task, __attribute__((unused)) gpointer obj,
                                __attribute__((unused)) gpointer data,
                                __attribute__((unused)) GCancellable *cancel)
{
    json_object *usb_list = discover_get_json();
    g_task_return_pointer(task, usb_list, (void *)json_object_put);
}

static void set_dev_state(gpointer old, gpointer new)
{
    UsbDesc *devold = old;
    UsbDesc *devnew = new;
    gboolean same_addr = FALSE;
    gboolean same_busid = FALSE;

    if (!g_strcmp0(usb_desc_get_node_addr(devold), usb_desc_get_node_addr(devnew))) {
        same_addr = TRUE;
    }

    if (!g_strcmp0(usb_desc_get_busid(devold), usb_desc_get_busid(devnew))) {
        same_busid = TRUE;
    }

    if ((same_addr & same_busid) == TRUE && usb_desc_get_state(devold) == FALSE) {
        usb_desc_set_state(devnew, FALSE);
        usb_desc_set_port(devnew, usb_desc_get_port(devold));
    }
}

static void usbip_app_win_refresh_list_done(GObject *app, GAsyncResult *res,
                                            __attribute__((unused)) gpointer data)
{
    g_return_if_fail(g_task_is_valid(res, app));

    UsbipAppWin *win = USBIP_APP_WIN(gtk_widget_get_toplevel(GTK_WIDGET(app)));
    UsbipAppWinPrivate *self = usbip_app_win_get_instance_private(win);

    json_object *usb_list = g_task_propagate_pointer(G_TASK(res), NULL);
    if (!json_object_object_length(usb_list)) {
        goto done;
    }

    gtk_container_foreach(GTK_CONTAINER(self->list_dev), (GtkCallback)clear_list_dev, NULL);

    GList *list = NULL;
    json_object *iter;
    json_object_object_foreach(usb_list, nodes, devices)
    {
        if (json_object_get_type(devices) == json_type_array) {
            for (size_t i = 0; i < json_object_array_length(devices); i++) {
                iter = json_object_array_get_idx(devices, i);

                UsbDesc *dev = g_object_new(USB_TYPE_DESC, "name", query_usb_id(iter, "product"),
                                            "id-vendor", query_usb_id(iter, "idVendor"),
                                            "id-product", query_usb_id(iter, "idProduct"),
                                            "manufacturer", query_usb_id(iter, "manufact"), "busid",
                                            query_usb_id(iter, "busid"), "node-addr", nodes, "port",
                                            NULL, "state", TRUE, NULL);

                g_list_foreach(self->devs, set_dev_state, dev);

                list = g_list_append(list, dev);
                GtkWidget *usb_entry_row = create_usbip_entry(dev);
                gtk_container_add(GTK_CONTAINER(self->list_dev), usb_entry_row);
            }
        }
    }

    g_list_free_full(self->devs, g_object_unref);
    self->devs = g_list_copy(list);
    g_list_free(list);
done:
    json_object_put(usb_list);
    gtk_widget_set_sensitive(self->button_add_dev, TRUE);
}

void usbip_app_win_refresh_list(UsbipAppWin *app)
{
    UsbipAppWin *win = USBIP_APP_WIN(gtk_widget_get_toplevel(GTK_WIDGET(app)));
    UsbipAppWinPrivate *self = usbip_app_win_get_instance_private(win);

    gtk_widget_set_sensitive(self->button_add_dev, FALSE);

    GTask *task = g_task_new(app, NULL, usbip_app_win_refresh_list_done, NULL);
    g_task_run_in_thread(task, refresh_list_thread);
    g_object_unref(task);
}

static void usbip_app_win_finalize(GObject *obj)
{
    UsbipAppWinPrivate *self = usbip_app_win_get_instance_private(USBIP_APP_WIN(obj));

    if (self->devs != NULL)
        g_list_free_full(self->devs, g_object_unref);

    G_OBJECT_CLASS(usbip_app_win_parent_class)->finalize(obj);
}

static void usbip_app_win_class_init(UsbipAppWinClass *class)
{
    G_OBJECT_CLASS(class)->finalize = usbip_app_win_finalize;

    gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(class),
                                                "/org/alunux/usbipapp/window.ui");
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), UsbipAppWin, win_scroll);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), UsbipAppWin,
                                                 button_add_dev);
    gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), usbip_app_win_refresh_list);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), UsbipAppWin, list_dev);
}

UsbipAppWin *usbip_app_win_new(UsbipApp *self)
{
    return g_object_new(USBIP_APP_WIN_TYPE, "application", self, NULL);
}
