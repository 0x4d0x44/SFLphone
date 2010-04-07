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

#include <addressbook.h>
#include <searchbar.h>
#include <string.h>
#include <addressbook-config.h>

static void
handler_async_search(GList *, gpointer);

/**
 * Perform a search on address book
 */
void
addressbook_search(GtkEntry* entry)
{

    const gchar* query = gtk_entry_get_text(GTK_ENTRY (entry));
    if (strlen(query) >= 3) {

        AddressBook_Config *addressbook_config;
	
	// Activate waiting layer
	activateWaitingLayer();

	// Load the address book parameters
	addressbook_config_load_parameters(&addressbook_config);
      
	// Start the asynchronous search as soon as we have an entry */
	search_async(gtk_entry_get_text(GTK_ENTRY (entry)), addressbook_config->max_results, &handler_async_search, addressbook_config);

    }
}

/**
 * Return addressbook state
 */
gboolean
addressbook_is_enabled()
{
  AddressBook_Config *addressbook_config;
  
  // Load the address book parameters
  addressbook_config_load_parameters(&addressbook_config);

  return (guint)addressbook_config->enable;
}

/**
 * Return addressbook state
 */
gboolean
addressbook_is_ready()
{
  return books_ready();
}

/**
 * Return TRUE if at least one addressbook is active
 */
gboolean
addressbook_is_active()
{
  return books_active();
}

/**
 * Asynchronous open callback.
 * Used to handle activation of books.
 */
static void
addressbook_config_books()
{

  gchar **config_book_uid;
  book_data_t *book_data;
  gchar **list;

  // Retrieve list of books
  list = (gchar **) dbus_get_addressbook_list();

  if (list) {

      for (config_book_uid = list; *config_book_uid; config_book_uid++) {

          // Get corresponding book data
          book_data = books_get_book_data_by_uid(*config_book_uid);

          // If book_data exists
          if (book_data != NULL) {

              book_data->active = TRUE;
	  }
      }
      g_strfreev(list);
  }

  // Update buttons
  update_actions ();
}

/**
 * Good method to get books_data
 */
GSList *
addressbook_get_books_data()
{
  addressbook_config_books();
  return books_data;
}

/**
 * Initialize books.
 * Set active/inactive status depending on config.
 */
void
addressbook_init()
{
  // Call books initialization
  init(&addressbook_config_books);
}

/**
 * Callback called after all book have been processed
 */
static void
handler_async_search(GList *hits, gpointer user_data)
{

  GList *i;
  GdkPixbuf *photo = NULL;
  AddressBook_Config *addressbook_config;
  callable_obj_t *j;

  // freeing calls
  while ((j = (callable_obj_t *) g_queue_pop_tail(contacts->callQueue)) != NULL)
    {
      free_callable_obj_t(j);
    }

  // Retrieve the address book parameters
  addressbook_config = (AddressBook_Config*) user_data;

  // reset previous results
  calltree_reset(contacts);
  calllist_reset(contacts);

  for (i = hits; i != NULL; i = i->next)
    {
      Hit *entry;
      entry = i->data;
      if (entry)
        {
          // Get the photo
          if (addressbook_display(addressbook_config,
              ADDRESSBOOK_DISPLAY_CONTACT_PHOTO))
            photo = entry->photo;
          // Create entry for business phone information
          if (addressbook_display(addressbook_config,
              ADDRESSBOOK_DISPLAY_PHONE_BUSINESS))
            calllist_add_contact(entry->name, entry->phone_business,
                CONTACT_PHONE_BUSINESS, photo);
          // Create entry for home phone information
          if (addressbook_display(addressbook_config,
              ADDRESSBOOK_DISPLAY_PHONE_HOME))
            calllist_add_contact(entry->name, entry->phone_home,
                CONTACT_PHONE_HOME, photo);
          // Create entry for mobile phone information
          if (addressbook_display(addressbook_config,
              ADDRESSBOOK_DISPLAY_PHONE_MOBILE))
            calllist_add_contact(entry->name, entry->phone_mobile,
                CONTACT_PHONE_MOBILE, photo);
        }
      free_hit(entry);
    }
  g_list_free(hits);

  // Deactivate waiting image
  deactivateWaitingLayer();
}

