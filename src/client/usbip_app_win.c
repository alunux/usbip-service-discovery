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
#include <netdb.h>

#include "discover.h"
#include "multicast_event.h"
#include "usbip.h"
#include "usbip_app.h"
#include "usbip_app_win.h"
#include "usbip_common.h"
#include "vhci_driver.h"

struct _UsbipAppWin {
    GtkApplicationWindow parent;
};

typedef struct _UsbipAppWinPrivate UsbipAppWinPrivate;

struct _UsbipAppWinPrivate {
    GtkWidget* nf_mess;
    GtkWidget* scrolled;
    GtkWidget* scan_result;
    GtkWidget* scan_button;
    GList* node_state;
    GList* node_list;
    gboolean cleared;
    json_object* usb_json;
};

G_DEFINE_TYPE_WITH_PRIVATE(UsbipAppWin,
                           usbip_app_win,
                           GTK_TYPE_APPLICATION_WINDOW)

static void
usbip_app_win_init(UsbipAppWin* win)
{
    gtk_widget_init_template(GTK_WIDGET(win));
}

static void
usbip_app_win_dispose(GObject* obj)
{
    UsbipAppWin* win = NULL;
    UsbipAppWinPrivate* priv = NULL;

    win = USBIP_APP_WIN(obj);
    priv = usbip_app_win_get_instance_private(win);

    g_clear_object(&priv->scan_result);

    G_OBJECT_CLASS(usbip_app_win_parent_class)->dispose(obj);
}

static void
usbip_app_win_finalize(GObject* obj)
{
    UsbipAppWin* win = NULL;
    UsbipAppWinPrivate* priv = NULL;

    win = USBIP_APP_WIN(obj);
    priv = usbip_app_win_get_instance_private(win);

    json_object_put(priv->usb_json);

    G_OBJECT_CLASS(usbip_app_win_parent_class)->finalize(obj);
}

static void
usbip_app_win_class_init(UsbipAppWinClass* class)
{
    G_OBJECT_CLASS(class)->dispose = usbip_app_win_dispose;
    G_OBJECT_CLASS(class)->finalize = usbip_app_win_finalize;

    gtk_widget_class_set_template_from_resource(
      GTK_WIDGET_CLASS(class), "/org/alunux/usbipapp/window.ui");

    gtk_widget_class_bind_template_child_private(
      GTK_WIDGET_CLASS(class), UsbipAppWin, scrolled);

    gtk_widget_class_bind_template_child_private(
      GTK_WIDGET_CLASS(class), UsbipAppWin, scan_button);

    gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class),
                                            usbip_app_win_update_list);

    gtk_widget_class_bind_template_child_private(
      GTK_WIDGET_CLASS(class), UsbipAppWin, scan_result);
}

UsbipAppWin*
usbip_app_win_new(UsbipApp* app)
{
    return g_object_new(USBIP_APP_WIN_TYPE, "application", app, NULL);
}

static GtkWidget*
control_usb_remote(GtkWidget* button, gpointer user_data)
{
    UsbipAppWin* win = USBIP_APP_WIN((g_list_first(user_data))->data);
    UsbipAppWinPrivate* priv = usbip_app_win_get_instance_private(win);

    UsbipAppDevice* usb = g_list_last(user_data)->data;

    int port, ret;
    char port_s[127];
    gchar* busid;

    busid = g_strdup(usb->busid);
    if (g_strcmp0(gtk_button_get_label(GTK_BUTTON(button)), "Attach") == 0) {
        port = attach_device(usb->node_addr, usb->busid);
        if (port < 0) {
            g_print("Can't attach %s at %s\n", usb->busid, usb->node_addr);
        } else {
            gtk_button_set_label(GTK_BUTTON(button), "Detach");
            usb->port = port;
            priv->node_state = g_list_append(priv->node_state, busid);
            g_print("Attach: %s at %s \n", usb->busid, usb->node_addr);
            g_print("VHCI portnum: %d\n", usb->port);
            if (announce_client_event() >= 0)
                g_print("Attach: Announcement success\n");
        }
    } else {
        snprintf(port_s, sizeof(port_s), "%d", usb->port);
        ret = detach_port(port_s);
        if (ret < 0) {
            g_print("Can't detach %s at %s from port %s\n",
                    usb->busid,
                    usb->node_addr,
                    port_s);
        } else {
            g_print("Detach: %s at %s\n", usb->busid, usb->node_addr);
            g_print("VHCI portnum: %d\n", usb->port);
            gtk_button_set_label(GTK_BUTTON(button), "Attach");
            priv->node_state = g_list_remove(priv->node_state, usb->busid);
            if (announce_client_event() >= 0)
                g_print("Detach: Announcement success\n");
        }
    }

    return button;
}

static GtkWidget*
usbip_app_win_get_usb_info(const gchar* node_addr,
                           json_object* usb_info,
                           UsbipAppWin* _win)
{
    UsbipAppWin* win = USBIP_APP_WIN(_win);
    UsbipAppWinPrivate* priv = usbip_app_win_get_instance_private(win);

    const gchar *product, *idProduct, *idVendor, *manufact, *busid;
    product = discover_query_usb_desc(usb_info, "product");
    idProduct = discover_query_usb_desc(usb_info, "idProduct");
    idVendor = discover_query_usb_desc(usb_info, "idVendor");
    manufact = discover_query_usb_desc(usb_info, "manufact");
    busid = discover_query_usb_desc(usb_info, "busid");

    UsbipAppDevice* node = g_new(UsbipAppDevice, 1);
    node->node_addr = node_addr;
    node->busid = busid;

    GList* item = NULL;
    item = g_list_append(item, win);
    item = g_list_append(item, node);

    priv->node_list = g_list_append(priv->node_list, node);

    GtkWidget* devs_info = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_show(devs_info);

    GtkWidget* button_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_show(button_box);

    gchar* devs_desc =
      g_strdup_printf("<b>%s</b>\nidVendor: %s\nidProduct: %s\nManufacturer: "
                      "%s\nBUSID: %s\nNode: %s\n",
                      product,
                      idVendor,
                      idProduct,
                      manufact,
                      busid,
                      node_addr);

    GtkWidget* label =
      gtk_label_new(discover_query_usb_desc(usb_info, "product"));
    gtk_label_set_markup(GTK_LABEL(label), devs_desc);
    gtk_widget_show(label);

    if (usbip_names_init(USBIDS_FILE)) {
        err("failed to open %s", USBIDS_FILE);
    }

    if (usbip_vhci_driver_open() < 0) {
        err("open vhci_driver");
        usbip_names_free();
        g_free(devs_desc);
        return NULL;
    }

    GtkWidget* button = NULL;
    struct usbip_imported_device* idev;
    char host[NI_MAXHOST] = "unknown host";
    char serv[NI_MAXSERV] = "unknown port";
    char remote_busid[SYSFS_BUS_ID_SIZE];
    int count_dev = 0;

    for (int i = 0; i < vhci_driver->nports; i++) {
        idev = &vhci_driver->idev[i];
        if (idev->status == VDEV_ST_NULL ||
            idev->status == VDEV_ST_NOTASSIGNED) {
            continue;
        }
        read_record(
          idev->port, host, sizeof(host), serv, sizeof(serv), remote_busid);
        g_print("addr: %s, busid: %s\n", host, remote_busid);
        if (check_device_state(node_addr, busid) < 0) {
            if (g_strcmp0(host, node_addr) == 0 &&
                g_strcmp0(remote_busid, busid) == 0) {
                button = gtk_button_new_with_label("Detach");
            }
        } else {
            button = gtk_button_new_with_label("In Used");
            gtk_widget_set_sensitive(button, FALSE);
        }
        count_dev++;
    }
    if (count_dev == 0) {
        button = gtk_button_new_with_label("Attach");
    }

    gtk_widget_show(button);
    gtk_box_set_center_widget(GTK_BOX(button_box), button);

    g_signal_connect(button, "clicked", G_CALLBACK(control_usb_remote), item);

    gtk_box_pack_start(GTK_BOX(devs_info), label, FALSE, FALSE, 0);
    gtk_box_pack_end(GTK_BOX(devs_info), button_box, FALSE, FALSE, 0);

    g_free(devs_desc);
    return devs_info;
}

static void
usbip_app_win_clear(UsbipAppWin* win)
{
    UsbipAppWinPrivate* priv = usbip_app_win_get_instance_private(win);

    GList* con_child =
      gtk_container_get_children(GTK_CONTAINER(priv->scrolled));
    for (GList* iter_con = con_child; iter_con != NULL;
         iter_con = g_list_next(iter_con)) {
        gtk_container_remove(GTK_CONTAINER(priv->scrolled),
                             GTK_WIDGET(iter_con->data));
    }
    g_list_free(con_child);

    gchar* no_mess = g_strdup("<span size=\"x-large\">There are no USB devices "
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
usbip_app_win_scan_done(GObject* source_object,
                        GAsyncResult* res,
                        gpointer user_data)
{
    UsbipAppWin* win = NULL;
    UsbipAppWinPrivate* priv = NULL;

    GtkWidget* devs_info = NULL;
    GList* iter_con = NULL;
    json_object* iterator = NULL;
    int count_dev = 0;

    win = USBIP_APP_WIN(source_object);
    priv = usbip_app_win_get_instance_private(win);

    json_object_put(priv->usb_json);
    priv->usb_json = g_task_propagate_pointer(G_TASK(res), NULL);

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
                  usbip_app_win_get_usb_info(node_addr, iterator, win);
                gtk_list_box_prepend(GTK_LIST_BOX(priv->scan_result),
                                     devs_info);
                count_dev++;
            }
        }
    }

    g_object_ref_sink(priv->scan_result);
    gtk_widget_show(priv->scan_result);

    if (count_dev == 0) {
        usbip_app_win_clear(win);
    } else if (priv->cleared) {
        gtk_container_remove(GTK_CONTAINER(priv->scrolled), priv->nf_mess);
        gtk_container_add(GTK_CONTAINER(priv->scrolled), priv->scan_result);
        priv->cleared = FALSE;
    }

    gtk_widget_set_sensitive(priv->scan_button, TRUE);
}

void
usbip_app_win_update_list(UsbipAppWin* _win)
{
    UsbipAppWin* win = NULL;
    UsbipAppWinPrivate* priv = NULL;
    GList* con_child = NULL;
    GTask* task_scan = NULL;

    win = USBIP_APP_WIN(gtk_widget_get_toplevel(GTK_WIDGET(_win)));
    priv = usbip_app_win_get_instance_private(win);
    gtk_widget_set_sensitive(priv->scan_button, FALSE);
    con_child = gtk_container_get_children(GTK_CONTAINER(priv->scan_result));

    task_scan = g_task_new(win, NULL, usbip_app_win_scan_done, con_child);
    g_task_run_in_thread(task_scan, discover_get_json);
    g_object_unref(task_scan);
}
