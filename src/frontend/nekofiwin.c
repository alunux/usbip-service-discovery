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
