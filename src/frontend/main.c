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

#include <errno.h>
#include <gtk/gtk.h>
#include <libudev.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "device.h"
#include "usbip.h"
#include "usbip_common.h"

static gboolean attach_usb_remote(void);
static gboolean detach_usb_remote(void);
static GtkWidget* interface_list_local(GtkWidget* widget, gpointer user_data);
static void main_window(GtkApplication* window, gpointer user_data);

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
    UsbDevice* USBDevInfo = (UsbDevice*)user_data;
    if (g_strcmp0(gtk_button_get_label(GTK_BUTTON(button)), "Attach") == 0) {
        if (attach_usb_remote()) {
            /* debugging purpose */
            printf("Attach: %s\n", USBDevInfo->product_usb);

            gtk_button_set_label(GTK_BUTTON(button), "Detach");
        }
    } else {
        if (detach_usb_remote()) {
            /* debugging purpose */
            printf("Detach: %s\n", USBDevInfo->product_usb);

            gtk_button_set_label(GTK_BUTTON(button), "Attach");
        }
    }
    return button;
}

static GtkWidget*
interface_list_local(GtkWidget* widget, gpointer user_data)
{
    GtkWidget* window = NULL;
    GtkWidget* devs_list = NULL;
    GtkWidget* button_box = NULL;
    GtkWidget* devs_info = NULL;
    GtkWidget* button = NULL;
    GtkWidget* label = NULL;
    gchar* devs_desc = NULL;
    GSList* usb_list = NULL;
    GSList* iterator = NULL;
    GList *children, *iter;

    window = (GtkWidget*)user_data;
    usb_list = usb_devices_list();
    devs_list = gtk_list_box_new();

    for (iterator = usb_list; iterator; iterator = iterator->next) {
        devs_info = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
        button_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

        devs_desc = g_strdup_printf("<b>%s</b>\nidVendor: %s\nidProduct: %s\n"
                                    "Manufacturer: %s\nBUSID: %s\n",
                                    ((UsbDevice*)iterator->data)->product_usb,
                                    ((UsbDevice*)iterator->data)->idVendor,
                                    ((UsbDevice*)iterator->data)->idProduct,
                                    ((UsbDevice*)iterator->data)->manufact,
                                    ((UsbDevice*)iterator->data)->busid);

        label = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(label), devs_desc);

        button = gtk_button_new_with_label("Attach");
        gtk_box_set_center_widget(GTK_BOX(button_box), button);
        g_signal_connect(button, "clicked", G_CALLBACK(control_usb_remote),
                         ((UsbDevice*)iterator->data));

        gtk_box_pack_start(GTK_BOX(devs_info), label, FALSE, FALSE, 0);
        gtk_box_pack_end(GTK_BOX(devs_info), button_box, FALSE, FALSE, 0);
        gtk_list_box_insert(GTK_LIST_BOX(devs_list), devs_info, 0);

        /* debugging purpose */
        g_print("product: %s\n", ((UsbDevice*)iterator->data)->product_usb);

        g_free(devs_desc);
    }

    gtk_container_add(GTK_CONTAINER(window), devs_list);
    g_slist_free(usb_list);
    /* debugging purpose */
    printf("Refresh list\n\n");
    return devs_list;
}

static void
refresh_list(GtkWidget* button, gpointer user_data)
{
    GtkWidget* refresh = (GtkWidget*)user_data;
    GList *children, *iter;

    children = gtk_container_get_children(GTK_CONTAINER(refresh));
    g_print("total widget = %d\n", g_list_length(children));
    if (children != NULL) {
        for (iter = children; iter != NULL; iter = g_list_next(iter)) {
            gtk_widget_destroy(GTK_WIDGET(iter->data));
        }

        g_list_free(children);
    }
}

static void
main_window(GtkApplication* app, gpointer user_data)
{
    GtkWidget* window = NULL;
    GtkWidget* header = NULL;
    GtkWidget* button = NULL;
    GtkWidget* box = NULL;
    GtkWidget* image = NULL;
    GtkWidget* list = NULL;
    GIcon* icon = NULL;
    gulong handler_id;

    window = gtk_application_window_new(app);

    header = gtk_header_bar_new();
    gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(header), TRUE);
    gtk_header_bar_set_title(GTK_HEADER_BAR(header),
                             "USB Device in Your Area - Skripsi");
    gtk_header_bar_set_has_subtitle(GTK_HEADER_BAR(header), FALSE);

    button = gtk_button_new();
    icon = g_themed_icon_new("view-refresh-symbolic");
    image = gtk_image_new_from_gicon(icon, GTK_ICON_SIZE_BUTTON);
    g_object_unref(icon);
    gtk_container_add(GTK_CONTAINER(button), image);
    gtk_header_bar_pack_end(GTK_HEADER_BAR(header), button);

    box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_style_context_add_class(gtk_widget_get_style_context(box), "linked");
    gtk_header_bar_pack_start(GTK_HEADER_BAR(header), box);

    gtk_window_set_titlebar(GTK_WINDOW(window), header);
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 400);

    list = interface_list_local(NULL, window);
    g_signal_connect(button, "clicked", G_CALLBACK(refresh_list), window);
    gtk_widget_show_all(window);
}

int
main(int argc, char* argv[])
{
    GtkApplication* app = NULL;
    int status = 0;

    app = gtk_application_new("org.alunux.usbip", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(main_window), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
