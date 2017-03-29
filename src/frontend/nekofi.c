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