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

#ifndef _USBIP_APP_WIN_H
#define _USBIP_APP_WIN_H

#include "usbip_app.h"
#include <gtk/gtk.h>

#define USBIP_APP_WIN_TYPE (usbip_app_win_get_type())

G_DECLARE_FINAL_TYPE(UsbipAppWin, usbip_app_win, USBIP_APP, WIN, GtkApplicationWindow)

UsbipAppWin *usbip_app_win_new(UsbipApp *app);
void usbip_app_win_refresh_list(UsbipAppWin *app);

#endif /* _USBIP_APP_WIN_H */
