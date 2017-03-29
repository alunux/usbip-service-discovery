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
}
