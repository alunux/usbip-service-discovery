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

static void
activate (GtkApplication *app, gpointer user_data)
{
  static GtkWidget *window = NULL;
  GtkWidget *header = NULL;
  GtkWidget *button = NULL;
  GtkWidget *box = NULL;
  GtkWidget *image = NULL;
  GIcon *icon = NULL;

  if (!window)
    {
      window = gtk_application_window_new (app);
      g_signal_connect (window, "destroy", G_CALLBACK (gtk_widget_destroyed),
                        &window);
      gtk_window_set_default_size (GTK_WINDOW (window), 600, 400);

      header = gtk_header_bar_new ();
      gtk_header_bar_set_show_close_button (GTK_HEADER_BAR (header), TRUE);
      gtk_header_bar_set_title (GTK_HEADER_BAR (header),
                                "USB Device in Your Area - Skripsi");
      gtk_header_bar_set_has_subtitle (GTK_HEADER_BAR (header), FALSE);

      button = gtk_button_new ();
      icon = g_themed_icon_new ("view-refresh-symbolic");
      image = gtk_image_new_from_gicon (icon, GTK_ICON_SIZE_BUTTON);
      g_object_unref (icon);
      gtk_container_add (GTK_CONTAINER (button), image);
      gtk_header_bar_pack_end (GTK_HEADER_BAR (header), button);

      box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
      gtk_style_context_add_class (gtk_widget_get_style_context (box),
                                   "linked");

      gtk_header_bar_pack_start (GTK_HEADER_BAR (header), box);
      gtk_window_set_titlebar (GTK_WINDOW (window), header);
    }

  if (!gtk_widget_get_visible (window))
    {
      gtk_widget_show_all (window);
    }
  else
    {
      gtk_widget_destroy (window);
    }
}

int
main (int argc, char *argv[])
{
  GtkApplication *app = NULL;
  int status;

  app = gtk_application_new ("org.alunux.skripsi", G_APPLICATION_FLAGS_NONE);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);

  return status;
}
