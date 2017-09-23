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

#ifndef DISCOVER_H
#define DISCOVER_H

#include <gio/gio.h>
#include <json.h>

#define UNUSED(...) (void)(__VA_ARGS__)

const char*
discover_query_usb_desc(json_object* root, const char* key);
void
discover_get_json(GTask* task,
                  gpointer source_obj,
                  gpointer task_data,
                  GCancellable* cancellable);

#endif /* DISCOVER_H */
