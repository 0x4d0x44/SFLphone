/*
 *  Copyright (C) 2009 Savoir-Faire Linux inc.
 *  Author: Julien Bonjean <julien.bonjean@savoirfairelinux.com>
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

#include <history.h>
#include <string.h>
#include <searchbar.h>
#include <calltab.h>

GtkTreeModel* history_filter;
GtkWidget * history_searchbar_widget;

static GtkTreeModel* history_create_filter (GtkTreeModel*);
static gboolean history_is_visible (GtkTreeModel*, GtkTreeIter*, gpointer);


void
history_search(GtkEntry* entry UNUSED){

  
  if(history_filter != NULL) {
    
    gtk_tree_model_filter_refilter(GTK_TREE_MODEL_FILTER(history_filter));
  
  }
}

void
history_init(){
  
  history_filter = history_create_filter(GTK_TREE_MODEL(history->store));
  gtk_tree_view_set_model(GTK_TREE_VIEW(history->view), GTK_TREE_MODEL(history_filter));
  
}

void history_set_searchbar_widget(GtkWidget *searchbar){

  history_searchbar_widget = searchbar;
}

static GtkTreeModel*
history_create_filter (GtkTreeModel* child) {

  GtkTreeModel* ret;
  GtkTreePath  *path; 

  DEBUG("Create Filter\n");
  ret = gtk_tree_model_filter_new(child, NULL);
  gtk_tree_model_filter_set_visible_func(GTK_TREE_MODEL_FILTER(ret), history_is_visible, NULL, NULL);
  return GTK_TREE_MODEL(ret);
}

static gboolean
history_is_visible (GtkTreeModel* model, GtkTreeIter* iter, gpointer data UNUSED) {

    if( SHOW_SEARCHBAR )
    {
        GValue val;

        gchar* text = NULL;
        gchar* search = (gchar*)gtk_entry_get_text(GTK_ENTRY(history_searchbar_widget));
        memset (&val, 0, sizeof(val));
        gtk_tree_model_get_value(GTK_TREE_MODEL(model), iter, 1, &val);
        if(G_VALUE_HOLDS_STRING(&val)){
            text = (gchar *)g_value_get_string(&val);
        }
        if(text != NULL && 
                ( g_ascii_strncasecmp(search, _("Search history"), 14) != 0 && g_ascii_strncasecmp(search, _("Search contact"), 14) != 0)){
            return g_regex_match_simple(search, text, G_REGEX_CASELESS, 0);
        }
        g_value_unset (&val);
        return TRUE;
    }

    return TRUE;
}
