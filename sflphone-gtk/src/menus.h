/*
 *  Copyright (C) 2007 Savoir-Faire Linux inc.
 *  Author: Pierre-Luc Beaudoin <pierre-luc@squidy.info>
 *                                                                              
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *                                                                                
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *                                                                              
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
 
#ifndef __MENUS_H__
#define __MENUS_H__

#include <gtk/gtk.h>

// These declaration must be in the .h so that mainwindow.c will see the variables
GtkWidget * webCamMenu;
guint webCamConnId;     //The webcam_menu signal connection ID

/** @file menus.h
  * @brief The menus of the main window.
  */
GtkWidget * create_menus();
void update_menus();
void show_popup_menu (GtkWidget *my_widget, GdkEventButton *event);
void menus_show_call_console_menu_item_set_active(gboolean active);

#endif 
