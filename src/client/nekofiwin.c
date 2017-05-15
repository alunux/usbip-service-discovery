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
#include "nekofi.h"
#include "nekofiwin.h"

struct _NekoFiWindow {
    GtkApplicationWindow parent;
};

typedef struct _NekoFiWindowPrivate NekoFiWindowPrivate;

struct _NekoFiWindowPrivate {
    GtkWidget* scan_result;
    json_object* usb_json;
};

G_DEFINE_TYPE_WITH_PRIVATE(NekoFiWindow, neko_fi_window,
                           GTK_TYPE_APPLICATION_WINDOW)

static void refresh_list(GtkWidget* scan_result, GParamSpec* pspec);

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
                                                 NekoFiWindow, scan_result);

    gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class),
                                            refresh_list);
}

NekoFiWindow*
neko_fi_window_new(NekoFi* app)
{
    return g_object_new(NEKO_FI_WINDOW_TYPE, "application", app, NULL);
}

static gboolean
attach_usb_remote(void)
{
    gboolean control_status = TRUE;
    return control_status;
}

static gboolean
detach_usb_remote(void)
{
    gboolean control_status = TRUE;
    return control_status;
}

static GtkWidget*
control_usb_remote(GtkWidget* button, gpointer user_data)
{
    gchar* product_usb = (gchar*)user_data;
    if (g_strcmp0(gtk_button_get_label(GTK_BUTTON(button)), "Attach") == 0) {
        if (attach_usb_remote()) {
            /* debugging purpose */
            g_print("Attach: %s\n", product_usb);
            gtk_button_set_label(GTK_BUTTON(button), "Detach");
        }
    } else {
        if (detach_usb_remote()) {
            /* debugging purpose */
            g_print("Detach: %s\n", product_usb);
            gtk_button_set_label(GTK_BUTTON(button), "Attach");
        }
    }
    return button;
}

static GtkWidget*
neko_fi_window_get_usb_info(gchar* node_addr, json_object* usb_info)
{
    GtkWidget* button_box = NULL;
    GtkWidget* devs_info = NULL;
    GtkWidget* button = NULL;
    GtkWidget* label = NULL;
    gchar* devs_desc = NULL;
    gchar* product = NULL;
    gchar* idProduct = NULL;
    gchar* idVendor = NULL;
    gchar* manufact = NULL;
    gchar* busid = NULL;

    product = get_usb_desc(usb_info, "product");
    idProduct = get_usb_desc(usb_info, "idVendor");
    idVendor = get_usb_desc(usb_info, "idProduct");
    manufact = get_usb_desc(usb_info, "manufact");
    busid = get_usb_desc(usb_info, "busid");

    devs_info = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_show(devs_info);
    g_object_ref_sink(devs_info);

    button_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_show(button_box);
    g_object_ref_sink(button_box);

    devs_desc =
      g_strdup_printf("<b>%s</b>\nidVendor: %s\nidProduct: %s\nManufacturer: "
                      "%s\nBUSID: %s\nNode: %s\n",
                      product, idVendor, idProduct, manufact, busid, node_addr);

    label = gtk_label_new(get_usb_desc(usb_info, "product"));
    gtk_label_set_markup(GTK_LABEL(label), devs_desc);
    gtk_widget_show(label);
    g_object_ref_sink(label);

    button = gtk_button_new_with_label("Attach");
    gtk_widget_show(button);
    gtk_box_set_center_widget(GTK_BOX(button_box), button);
    g_object_ref_sink(button);

    g_signal_connect(button, "clicked", G_CALLBACK(control_usb_remote),
                     get_usb_desc(usb_info, "product"));

    gtk_box_pack_start(GTK_BOX(devs_info), label, FALSE, FALSE, 0);
    gtk_box_pack_end(GTK_BOX(devs_info), button_box, FALSE, FALSE, 0);

    g_free(devs_desc);
    return devs_info;
}

void
neko_fi_window_scan(NekoFiWindow* win)
{
    NekoFiWindowPrivate* priv = NULL;

    GtkWidget* devs_info = NULL;
    json_object* iterator = NULL;

    priv = neko_fi_window_get_instance_private(win);
    priv->usb_json = nekofi_discover_json();

    json_object_object_foreach(priv->usb_json, node_addr, devices)
    {
        if (json_object_get_type(devices) == json_type_array) {
            for (int i = 0; i < json_object_array_length(devices); i++) {
                iterator = json_object_array_get_idx(devices, i);
                devs_info = append_usb_info(node_addr, iterator);
                gtk_list_box_prepend(GTK_LIST_BOX(priv->scan_result),
                                     devs_info);
            }
        }
    }

    g_object_ref_sink(priv->scan_result);
}

static void
refresh_list(GtkWidget* scan_result, GParamSpec* pspec)
{
    NekoFiWindow* win = NULL;
    NekoFiWindowPrivate* priv = NULL;
    GList* con_child = NULL;
    GList* iter_con = NULL;

    if (gtk_widget_in_destruction(scan_result))
        return;

    win = NEKO_FI_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(scan_result)));
    priv = neko_fi_window_get_instance_private(win);
    con_child = gtk_container_get_children(GTK_CONTAINER(priv->scan_result));

    for (iter_con = con_child; iter_con != NULL;
         iter_con = g_list_next(iter_con)) {
        gtk_container_remove(GTK_CONTAINER(priv->scan_result),
                             GTK_WIDGET(iter_con->data));
    }

    g_list_free(con_child);
    json_object_put(priv->usb_json);

    neko_fi_window_scan(win);
}
