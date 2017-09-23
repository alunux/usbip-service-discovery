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

#ifndef NEKOFI_H
#define NEKOFI_H

#include <gtk/gtk.h>

#define NEKO_FI_TYPE (neko_fi_get_type())

G_DECLARE_FINAL_TYPE(NekoFi, neko_fi, NEKO, FI, GtkApplication)

NekoFi*
neko_fi_new(void);

#endif /* NEKOFI_H */
