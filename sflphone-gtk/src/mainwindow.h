/*
 *  Copyright (C) 2007 Savoir-Faire Linux inc.
 *  Author: Pierre-Luc Beaudoin <pierre-luc.beaudoin@savoirfairelinux.com>
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
 
#ifndef __MAINWINDOW_H__
#define __MAINWINDOW_H__

#include <calllist.h>

/** @file mainwindow.h
  * @brief The main window of the client.
  */
GtkAccelGroup * get_accel_group();

/**
 * Display the main window
 * @return GtkWidget* The main window
 */
GtkWidget * get_main_window();

/**
 * Build the main window
 */
void create_main_window ( );

/**
 * Display a dialog window 
 * Ask the user if he wants to hangup current calls before quiting
 * @return gboolean TRUE if the user wants to hang up
 *		    FALSE otherwise
 */ 
gboolean main_window_ask_quit() ;

/**
  * Shows/Hides the dialpad on the mainwindow 
  */
void main_window_dialpad( gboolean* state );

/**
  * Shows/Hides the dialpad on the mainwindow 
  */
void main_window_volume_controls( gboolean* state );

void main_window_show_call_console(gboolean show);

/**
 * Display an error message
 * @param markup  The error message
 */
void main_window_error_message(gchar * markup);

/**
 * Display a warning message
 * @param markup  The warning message
 */
void main_window_warning_message(gchar * markup);

/**
 * Display an info message
 * @param markup  The info message
 */
void main_window_info_message(gchar * markup);

/**
 * Push a message on the statusbar stack
 * @param message The message to display
 * @param id  The identifier of the message
 */
void statusbar_push_message( const gchar* message , guint id );

/**
 * Pop a message from the statusbar stack
 * @param id  The identifier of the message
 */
void statusbar_pop_message( guint id );

/**
 * Shows the OpenGl visualization window
 * @param show TRUE if you want to show the dialpad, FALSE to hide it
 */
gboolean main_window_glWidget( gboolean show );

/**
 * Keeps both the button and the menu item at the same value
 * @param value the value to wich the button and the menu item must be set
 */
void main_window_update_WebcamStatus( gboolean value );

gboolean get_showGlWidget_status();

void main_window_call_console_closed();


#endif 
