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

#include "discover.h"
#include "multicast_event.h"
#include "nekofi.h"
#include "nekofiwin.h"
#include "usbip.h"

struct _NekoFiWindow {
    GtkApplicationWindow parent;
};

typedef struct _NekoFiWindowPrivate NekoFiWindowPrivate;

struct _NekoFiWindowPrivate {
    GtkWidget* nf_mess;
    GtkWidget* scrolled;
    GtkWidget* scan_result;
    GList* node_state;
    gboolean cleared;
    json_object* usb_json;
    NekoFiDevice* dev_tmp;
};

G_DEFINE_TYPE_WITH_PRIVATE(NekoFiWindow, neko_fi_window,
                           GTK_TYPE_APPLICATION_WINDOW)

static void
neko_fi_window_init(NekoFiWindow* win)
{
    gtk_widget_init_template(GTK_WIDGET(win));
}

static void
neko_fi_window_dispose(GObject* object)
{
    NekoFiWindow* win = NULL;
    NekoFiWindowPrivate* priv = NULL;

    win = NEKO_FI_WINDOW(object);
    priv = neko_fi_window_get_instance_private(win);

    g_clear_object(&priv->scan_result);

    G_OBJECT_CLASS(neko_fi_window_parent_class)->dispose(object);
}

static void
neko_fi_window_finalize(GObject* object)
{
    NekoFiWindow* win = NULL;
    NekoFiWindowPrivate* priv = NULL;

    win = NEKO_FI_WINDOW(object);
    priv = neko_fi_window_get_instance_private(win);

    json_object_put(priv->usb_json);

    G_OBJECT_CLASS(neko_fi_window_parent_class)->finalize(object);
}

static void
neko_fi_window_class_init(NekoFiWindowClass* class)
{
    G_OBJECT_CLASS(class)->dispose = neko_fi_window_dispose;
    G_OBJECT_CLASS(class)->finalize = neko_fi_window_finalize;

    gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(class),
                                                "/org/alunux/nekofi/window.ui");

    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class),
                                                 NekoFiWindow, scrolled);

    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class),
                                                 NekoFiWindow, scan_result);
}

NekoFiWindow*
neko_fi_window_new(NekoFi* app)
{
    return g_object_new(NEKO_FI_WINDOW_TYPE, "application", app, NULL);
}

static GtkWidget*
neko_fi_window_control_usb_remote(GtkWidget* button, gpointer user_data)
{
    NekoFiWindow* win = NULL;
    NekoFiWindowPrivate* priv = NULL;

    win = NEKO_FI_WINDOW(user_data);
    priv = neko_fi_window_get_instance_private(win);

    int port, ret;
    char port_s[127];
    gchar* busid;

    busid = g_strdup(priv->dev_tmp->busid);

    if (g_strcmp0(gtk_button_get_label(GTK_BUTTON(button)), "Attach") == 0) {
        port = attach_device(priv->dev_tmp->node_addr, priv->dev_tmp->busid);
        if (port < 0) {
            g_print("Can't attach %s at %s\n", priv->dev_tmp->busid,
                    priv->dev_tmp->node_addr);
        } else {
            g_print("Attach: %s at %s\n", priv->dev_tmp->busid,
                    priv->dev_tmp->node_addr);
            gtk_button_set_label(GTK_BUTTON(button), "Detach");
            priv->dev_tmp->port = port;
            priv->node_state = g_list_append(priv->node_state, busid);
            announce_client_event();
        }
    } else {
        snprintf(port_s, sizeof(port_s), "%d", priv->dev_tmp->port);
        ret = detach_port(port_s);
        if (ret < 0) {
            g_print("Can't detach %s at %s\n", priv->dev_tmp->busid,
                    priv->dev_tmp->node_addr);
        } else {
            g_print("Detach: %s at %s\n", priv->dev_tmp->busid,
                    priv->dev_tmp->node_addr);
            gtk_button_set_label(GTK_BUTTON(button), "Attach");
            priv->node_state = g_list_remove(priv->node_state, busid);
            announce_client_event();
        }
    }

    return button;
}

static GtkWidget*
neko_fi_window_get_usb_info(gchar* node_addr, json_object* usb_info,
                            NekoFiWindow* _win)
{
    NekoFiWindow* win = NULL;
    NekoFiWindowPrivate* priv = NULL;
    GtkWidget* button_box = NULL;
    GtkWidget* devs_info = NULL;
    GtkWidget* button = NULL;
    GtkWidget* label = NULL;
    GList* iter_con = NULL;
    gchar* devs_desc;
    gchar* product;
    gchar* idProduct;
    gchar* idVendor;
    gchar* manufact;
    gchar* busid;

    product = discover_query_usb_desc(usb_info, "product");
    idProduct = discover_query_usb_desc(usb_info, "idProduct");
    idVendor = discover_query_usb_desc(usb_info, "idVendor");
    manufact = discover_query_usb_desc(usb_info, "manufact");
    busid = discover_query_usb_desc(usb_info, "busid");

    win = NEKO_FI_WINDOW(_win);
    priv = neko_fi_window_get_instance_private(win);

    priv->dev_tmp = g_new(NekoFiDevice, 1);
    priv->dev_tmp->node_addr = node_addr;
    priv->dev_tmp->busid = busid;

    devs_info = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_show(devs_info);

    button_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_show(button_box);

    devs_desc =
      g_strdup_printf("<b>%s</b>\nidVendor: %s\nidProduct: %s\nManufacturer: "
                      "%s\nBUSID: %s\nNode: %s\n",
                      product, idVendor, idProduct, manufact, busid, node_addr);

    label = gtk_label_new(discover_query_usb_desc(usb_info, "product"));
    gtk_label_set_markup(GTK_LABEL(label), devs_desc);
    gtk_widget_show(label);

    if (check_device_state(node_addr, busid) < 0) {
        if (priv->node_state != NULL) {
            for (iter_con = priv->node_state; iter_con != NULL;
                 iter_con = iter_con->next) {
                if (g_strcmp0(iter_con->data, busid) == 0) {
                    button = gtk_button_new_with_label("Detach");
                }
            }
        } else {
            button = gtk_button_new_with_label("In Used");
            gtk_widget_set_sensitive(button, FALSE);
        }
    } else {
        button = gtk_button_new_with_label("Attach");
    }

    gtk_widget_show(button);
    gtk_box_set_center_widget(GTK_BOX(button_box), button);

    g_signal_connect(button, "clicked",
                     G_CALLBACK(neko_fi_window_control_usb_remote), win);

    gtk_box_pack_start(GTK_BOX(devs_info), label, FALSE, FALSE, 0);
    gtk_box_pack_end(GTK_BOX(devs_info), button_box, FALSE, FALSE, 0);

    g_free(devs_desc);
    return devs_info;
}

static void
neko_fi_window_clear(NekoFiWindow* win)
{
    NekoFiWindowPrivate* priv = NULL;
    GList* con_child = NULL;
    GList* iter_con = NULL;
    gchar* no_mess = NULL;

    priv = neko_fi_window_get_instance_private(win);
    con_child = gtk_container_get_children(GTK_CONTAINER(priv->scrolled));

    for (iter_con = con_child; iter_con != NULL;
         iter_con = g_list_next(iter_con)) {
        gtk_container_remove(GTK_CONTAINER(priv->scrolled),
                             GTK_WIDGET(iter_con->data));
    }

    g_list_free(con_child);

    no_mess = g_strdup("<span size=\"x-large\">There are no USB devices "
                       "found\nin your area . . .</span>");

    priv->nf_mess = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(priv->nf_mess), no_mess);
    gtk_label_set_justify(GTK_LABEL(priv->nf_mess), GTK_JUSTIFY_CENTER);
    g_free(no_mess);
    gtk_widget_show(priv->nf_mess);
    gtk_container_add(GTK_CONTAINER(priv->scrolled), priv->nf_mess);

    priv->cleared = TRUE;
}

static void
neko_fi_window_scan_done(GObject* source_object, GAsyncResult* res,
                         gpointer user_data)
{
    NekoFiWindow* win = NULL;
    NekoFiWindowPrivate* priv = NULL;

    GtkWidget* devs_info = NULL;
    GList* iter_con = NULL;
    json_object* iterator = NULL;
    int count_dev = 0;

    win = NEKO_FI_WINDOW(source_object);
    priv = neko_fi_window_get_instance_private(win);

    json_object_put(priv->usb_json);
    priv->usb_json = g_task_propagate_pointer(G_TASK(res), NULL);
    if (priv->usb_json == NULL) {
        printf("Ada error bro\n");
    }

    for (iter_con = user_data; iter_con != NULL;
         iter_con = g_list_next(iter_con)) {
        gtk_container_remove(GTK_CONTAINER(priv->scan_result),
                             GTK_WIDGET(iter_con->data));
    }

    g_list_free(user_data);

    json_object_object_foreach(priv->usb_json, node_addr, devices)
    {
        if (json_object_get_type(devices) == json_type_array) {
            for (int i = 0; i < json_object_array_length(devices); i++) {
                iterator = json_object_array_get_idx(devices, i);
                devs_info =
                  neko_fi_window_get_usb_info(node_addr, iterator, win);
                gtk_list_box_prepend(GTK_LIST_BOX(priv->scan_result),
                                     devs_info);
                count_dev++;
            }
        }
    }

    g_object_ref_sink(priv->scan_result);
    gtk_widget_show(priv->scan_result);

    if (count_dev == 0) {
        neko_fi_window_clear(win);
    } else if (priv->cleared) {
        gtk_container_remove(GTK_CONTAINER(priv->scrolled), priv->nf_mess);
        gtk_container_add(GTK_CONTAINER(priv->scrolled), priv->scan_result);
        priv->cleared = FALSE;
    }
}

void
neko_fi_window_update_list(NekoFiWindow* _win)
{
    NekoFiWindow* win = NULL;
    NekoFiWindowPrivate* priv = NULL;
    GList* con_child = NULL;
    GTask* task_scan = NULL;

    win = NEKO_FI_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(_win)));
    priv = neko_fi_window_get_instance_private(win);
    con_child = gtk_container_get_children(GTK_CONTAINER(priv->scan_result));

    task_scan = g_task_new(win, NULL, neko_fi_window_scan_done, con_child);
    g_task_run_in_thread(task_scan, discover_get_json);
    g_object_unref(task_scan);
}
