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
#include <stdio.h>
#include <stdlib.h>

static GtkWidget*
usbip_header_bar(const gchar* title)
{
    GtkWidget* header = NULL;
    GtkWidget* button = NULL;
    GtkWidget* box = NULL;
    GtkWidget* image = NULL;
    GIcon* icon = NULL;

    header = gtk_header_bar_new();
    gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(header), TRUE);
    gtk_header_bar_set_title(GTK_HEADER_BAR(header), title);
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
    return header;
}

static GtkWidget*
usb_devices_list()
{
    GtkWidget* devs_list = NULL;
    GtkWidget* devs_info = NULL;
    GtkWidget* button = NULL;
    GtkWidget* label = NULL;
    gchar* devs_desc = NULL;

    devs_list = gtk_list_box_new();

    /*
    * Testing purpose
    */
    for (int i = 1; i < 10; i++) {
        devs_info = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
        devs_desc = g_strdup_printf("USB Device %i", i);
        label = gtk_label_new(devs_desc);
        button = gtk_button_new_with_label("Connect");
        gtk_container_add(GTK_CONTAINER(devs_list), devs_info);
        gtk_container_add(GTK_CONTAINER(devs_info), label);
        gtk_container_add(GTK_CONTAINER(devs_info), button);
        gtk_button_box_set_layout(GTK_BUTTON_BOX(devs_info),
                                  GTK_BUTTONBOX_EDGE);
    }
    g_free(devs_desc);
    return devs_list;
}

static GtkWidget*
main_window(GtkWidget* window)
{
    GtkWidget* header = NULL;

    if (!window) {
        window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        g_signal_connect(window, "destroy", G_CALLBACK(gtk_widget_destroyed),
                         &window);

        header = usbip_header_bar("USB Device in Your Area - Skripsi");
        gtk_window_set_default_size(GTK_WINDOW(window), 600, 400);
        gtk_window_set_titlebar(GTK_WINDOW(window), header);

        gtk_container_add(GTK_CONTAINER(window), usb_devices_list());
    }
    return window;
}

int
main(int argc, char* argv[])
{
    GtkWidget* window = NULL;

    gtk_init(&argc, &argv);
    window = main_window(window);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_widget_show_all(window);
    gtk_main();

    return EXIT_SUCCESS;
}
