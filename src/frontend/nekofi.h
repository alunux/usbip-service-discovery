#ifndef __NEKOFI_H
#define __NEKOFI_H

#include <gtk/gtk.h>


#define NEKO_FI_TYPE (neko_fi_get_type ())
G_DECLARE_FINAL_TYPE (NekoFi, neko_fi, NEKO, FI, GtkApplication)

NekoFi *neko_fi_new(void);


#endif /* __NEKOFI_H */