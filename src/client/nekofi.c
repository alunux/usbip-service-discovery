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

#include <gio/gio.h>
#include <gtk/gtk.h>

#include "detect_iface.h"
#include "nekofi.h"
#include "nekofiwin.h"

#define NEKOFI_CAST_ADDR "225.10.10.1"
#define LISTENPORT 10297

struct _NekoFi {
    GtkApplication parent;
};

G_DEFINE_TYPE(NekoFi, neko_fi, GTK_TYPE_APPLICATION)

static void
neko_fi_init(NekoFi* app)
{
}

static gboolean
gio_read_socket(GIOChannel* channel, GIOCondition condition, gpointer data)
{
    NekoFiWindow* win = NULL;
    GError* error = NULL;
    char buf[1024];
    gsize bytes_read;

    if (condition & G_IO_HUP)
        return FALSE; /* this channel is done */

    g_io_channel_read_chars(channel, buf, sizeof(buf), &bytes_read, &error);
    g_assert(error == NULL);
    buf[bytes_read] = '\0';

    g_print("Ada perubahan device\n");

    win = NEKO_FI_WINDOW(data);
    neko_fi_window_update_list(win);

    return TRUE;
}

static void
neko_fi_activate(GApplication* app)
{
    NekoFiWindow* win = NULL;
    GSocket* sock_event = NULL;
    GInetAddress* inetaddr = NULL;
    GSocketAddress* sockaddr = NULL;
    GError* err = NULL;

    inetaddr = g_inet_address_new_any(G_SOCKET_FAMILY_IPV4);
    sockaddr =
      G_SOCKET_ADDRESS(g_inet_socket_address_new(inetaddr, LISTENPORT));

    sock_event = g_socket_new(G_SOCKET_FAMILY_IPV4, G_SOCKET_TYPE_DATAGRAM,
                              G_SOCKET_PROTOCOL_UDP, &err);

    if (sock_event == NULL) {
        g_print("g_socket_new: %s\n", err->message);
        exit(1);
    }

    if (!g_socket_bind(sock_event, sockaddr, TRUE, &err)) {
        g_print("g_socket_bind: error\n");
        exit(1);
    }

    if (!g_socket_join_multicast_group(
          sock_event, g_inet_address_new_from_string(NEKOFI_CAST_ADDR), FALSE,
          find_wifi_interface(), NULL)) {
        g_print("g_socket_join_multicast_group: error\n");
    }

    int fd = g_socket_get_fd(sock_event);

    win = neko_fi_window_new(NEKO_FI(app));
    neko_fi_window_update_list(win);
    gtk_window_present(GTK_WINDOW(win));

    GIOChannel* channel = g_io_channel_unix_new(fd);
    g_io_add_watch(channel, G_IO_IN, (GIOFunc)gio_read_socket, win);
    g_io_channel_unref(channel);
}

static void
neko_fi_class_init(NekoFiClass* class)
{
    G_APPLICATION_CLASS(class)->activate = neko_fi_activate;
}

NekoFi*
neko_fi_new(void)
{
    return g_object_new(NEKO_FI_TYPE, "application-id", "org.alunux.nekofi",
                        "flags", G_APPLICATION_FLAGS_NONE, NULL);
}
