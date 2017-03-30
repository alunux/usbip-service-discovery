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

#include "nekofi.h"
#include "nekofiwin.h"

struct _NekoFiWindow
{
    GtkApplicationWindow parent;
};

typedef struct _NekoFiWindowPrivate NekoFiWindowPrivate;

struct _NekoFiWindowPrivate
{
    GtkWidget* scan_result;
};

G_DEFINE_TYPE_WITH_PRIVATE(NekoFiWindow, neko_fi_window, GTK_TYPE_APPLICATION_WINDOW);

static void
neko_fi_window_init(NekoFiWindow *win)
{
    gtk_widget_init_template(GTK_WIDGET (win));
}

static void
neko_fi_window_class_init(NekoFiWindowClass *class)
{
    gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (class),
                                               "/org/alunux/nekofi/window.ui");
    gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (class), NekoFiWindow, scan_result);
}

NekoFiWindow*
neko_fi_window_new(NekoFi *app)
{
    return g_object_new(NEKO_FI_WINDOW_TYPE, "application", app, NULL);
}

void neko_fi_window_scan(NekoFiWindow *win)
{
    NekoFiWindowPrivate *priv = NULL;

    GtkWidget* button_box = NULL;
    GtkWidget* devs_info = NULL;
    GtkWidget* button = NULL;
    GtkWidget* label = NULL;
    gchar* devs_desc = NULL;
    GSList* items = NULL;
    GSList* iterator = NULL;

    priv = neko_fi_window_get_instance_private (win);
    items = usb_devices_list();

    for (iterator = items; iterator; iterator = iterator->next) {
        devs_info = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
        gtk_widget_show(devs_info);
        button_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
        gtk_widget_show(button_box);

        devs_desc = g_strdup_printf("<b>%s</b>\nidVendor: %s\nidProduct: %s\n"
                                    "Manufacturer: %s\nBUSID: %s\n",
                                    ((UsbDevice*)iterator->data)->product_usb,
                                    ((UsbDevice*)iterator->data)->idVendor,
                                    ((UsbDevice*)iterator->data)->idProduct,
                                    ((UsbDevice*)iterator->data)->manufact,
                                    ((UsbDevice*)iterator->data)->busid);

        label = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(label), devs_desc);
        gtk_widget_show(label);

        button = gtk_button_new_with_label("Attach");
        gtk_widget_show(button);
        gtk_box_set_center_widget(GTK_BOX(button_box), button);

        /*
        g_signal_connect(button, "clicked", G_CALLBACK(control_usb_remote),
                         ((UsbDevice*)iterator->data));
        */

        gtk_box_pack_start(GTK_BOX(devs_info), label, FALSE, FALSE, 0);
        gtk_box_pack_end(GTK_BOX(devs_info), button_box, FALSE, FALSE, 0);
        gtk_list_box_insert(GTK_LIST_BOX(priv->scan_result), devs_info, 0);

        /* debugging purpose */
        g_print("product: %s\n", ((UsbDevice*)iterator->data)->product_usb);

        g_free(devs_desc);
    }

    for (iterator = items; iterator; iterator = iterator->next) {
        finish_dev_usage(((UsbDevice*)iterator->data)->dev);
    }

    g_free(items->data);
    g_slist_free(items);
}
