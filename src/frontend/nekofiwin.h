#ifndef __NEKOFIWIN_H
#define __NEKOFIWIN_H

#include <gtk/gtk.h>
#include "nekofi.h"
#include "device.h"


#define NEKO_FI_WINDOW_TYPE (neko_fi_window_get_type ())
G_DECLARE_FINAL_TYPE (NekoFiWindow, neko_fi_window, NEKO, FI_WINDOW, GtkApplicationWindow)


NekoFiWindow *neko_fi_window_new(NekoFi *app);
void neko_fi_window_scan(NekoFiWindow *win);


#endif /* __NEKOFIWIN_H */
