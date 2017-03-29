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
#include "device.h"

struct _NekoFi
{
    GtkApplication parent;
};

G_DEFINE_TYPE(NekoFi, neko_fi, GTK_TYPE_APPLICATION);

static void
neko_fi_init(NekoFi *app)
{
}

static void
neko_fi_activate(GApplication *app)
{
    NekoFiWindow *win = NULL;

    win = neko_fi_window_new(NEKO_FI(app));
    gtk_window_present(GTK_WINDOW(win));
}

static void
neko_fi_scan(GApplication *app, UsbDevice *list)
{
    NekoFiWindow *win = NULL;
}

static void
neko_fi_class_init(NekoFiClass *class)
{
    G_APPLICATION_CLASS(class)->activate = neko_fi_activate;
}

NekoFi*
neko_fi_new(void)
{
    return g_object_new(NEKO_FI_TYPE,
                        "application-id", "org.alunux.nekofi",
                        "flags", NULL,
                        NULL);
}
