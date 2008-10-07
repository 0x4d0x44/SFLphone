/*
 *  Copyright (C) 2007-2008 Savoir-Faire Linux inc.
 *  Author: Emmanuel Milou <emmanuel.milou@savoirfairelinux.com>
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

#include <actions.h>
#include <mainwindow.h>
#include <accountlist.h>

#include <libsexy/sexy-icon-entry.h>
#include <string.h>
#include <dbus.h>
#include <config.h>
#include <gtk/gtk.h>

/** Local variables */
account_t * currentAccount;

GtkDialog * dialog;
GtkWidget * hbox;
GtkWidget * frame;
GtkWidget * frameNat;
GtkWidget * table;
GtkWidget * tableNat;
GtkWidget * label;
GtkWidget * entryID;
GtkWidget * entryAlias;
GtkWidget * entryProtocol;
GtkWidget * entryEnabled;
GtkWidget * entryUsername;
GtkWidget * entryHostname;
GtkWidget * entryPort;
GtkWidget * entryPassword;
GtkWidget * stunServer;
GtkWidget * stunEnable;
GtkWidget * entryMailbox;

/* Signal to entryProtocol 'changed' */
  void
change_protocol (account_t * currentAccount)
{
  gchar* proto = (gchar *)gtk_combo_box_get_active_text(GTK_COMBO_BOX(entryProtocol));
  //g_print("Protocol changed\n");

  // toggle sensitivity for: entryUserPart 
  if (strcmp(proto, "SIP") == 0) {
    gtk_widget_set_sensitive( GTK_WIDGET(stunEnable), TRUE);
    gtk_widget_set_sensitive( GTK_WIDGET(stunServer), TRUE);
    gtk_widget_set_sensitive( GTK_WIDGET(entryPort),  TRUE);
  }
  else if (strcmp(proto, "IAX") == 0) {
    gtk_widget_set_sensitive( GTK_WIDGET(stunEnable),   FALSE);
    gtk_widget_set_sensitive( GTK_WIDGET(stunServer),   FALSE);
    gtk_widget_set_sensitive( GTK_WIDGET(entryPort),   FALSE);
  }
  else {
    // Should not get here.
    g_print("Unknown protocol: %s\n", proto);
  }
}

  int 
is_iax_enabled(void)
{
  int res = dbus_is_iax2_enabled();
  if(res == 1)	
    return TRUE;
  else	
    return FALSE;
}

void
stun_state( void )
{
  gboolean stunActive = (gboolean)gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( stunEnable ));
  gtk_widget_set_sensitive( GTK_WIDGET( stunServer ) , stunActive );
}


  void
show_account_window (account_t * a)
{
  guint response;
  GtkWidget *image;

  currentAccount = a;

  // Default settings
  gchar * curAccountID = "";
  gchar * curAccountEnabled = "TRUE";
  gchar * curAccountType = "SIP";
  gchar * curAlias = "";
  gchar * curUsername = "";
  gchar * curHostname = "";
  gchar * curPort = "5060";
  gchar * curPassword = "";
  /* TODO: add curProxy, and add boxes for Proxy support */
  gchar * stun_enabled = "FALSE";
  gchar * stun_server= "stun.fwdnet.net:3478";
  gchar * curMailbox = "888";

  // Load from SIP/IAX/Unknown ?
  if(a)
  {
    curAccountID = a->accountID;
    curAccountType = g_hash_table_lookup(currentAccount->properties, ACCOUNT_TYPE);
    curAccountEnabled = g_hash_table_lookup(currentAccount->properties, ACCOUNT_ENABLED);
    curAlias = g_hash_table_lookup(currentAccount->properties, ACCOUNT_ALIAS);

    if (strcmp(curAccountType, "IAX") == 0) {
      curHostname = g_hash_table_lookup(currentAccount->properties, ACCOUNT_IAX_HOST);
      curPassword = g_hash_table_lookup(currentAccount->properties, ACCOUNT_IAX_PASSWORD);
      curUsername = g_hash_table_lookup(currentAccount->properties, ACCOUNT_IAX_USER);
      curMailbox = g_hash_table_lookup(currentAccount->properties, ACCOUNT_MAILBOX);
    }
    else if (strcmp(curAccountType, "SIP") == 0) {
      curHostname = g_hash_table_lookup(currentAccount->properties, ACCOUNT_SIP_HOST);
      curPort = g_hash_table_lookup(currentAccount->properties, ACCOUNT_SIP_PORT);
      curPassword = g_hash_table_lookup(currentAccount->properties, ACCOUNT_SIP_PASSWORD);
      curUsername = g_hash_table_lookup(currentAccount->properties, ACCOUNT_SIP_USER);
      stun_enabled = g_hash_table_lookup(currentAccount->properties, ACCOUNT_SIP_STUN_ENABLED);
      stun_server = g_hash_table_lookup(currentAccount->properties, ACCOUNT_SIP_STUN_SERVER);
      curMailbox = g_hash_table_lookup(currentAccount->properties, ACCOUNT_MAILBOX);
    }
  }
  else
  {
    currentAccount = g_new0(account_t, 1);
    currentAccount->properties = g_hash_table_new(NULL, g_str_equal);
    curAccountID = "test";
  }

  dialog = GTK_DIALOG(gtk_dialog_new_with_buttons (_("Account settings"),
	GTK_WINDOW(get_main_window()),
	GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
	GTK_STOCK_APPLY,
	GTK_RESPONSE_ACCEPT,
	GTK_STOCK_CANCEL,
	GTK_RESPONSE_CANCEL,
	NULL));

  gtk_dialog_set_has_separator(dialog, TRUE);
  gtk_container_set_border_width (GTK_CONTAINER(dialog), 0);

  frame = gtk_frame_new(_("Account parameters"));
  gtk_box_pack_start(GTK_BOX(dialog->vbox), frame, FALSE, FALSE, 0);
  gtk_widget_show(frame);

  table = gtk_table_new ( 9, 2  ,  FALSE/* homogeneous */);
  gtk_table_set_row_spacings( GTK_TABLE(table), 10);
  gtk_table_set_col_spacings( GTK_TABLE(table), 10);
  gtk_widget_show(table);
  gtk_container_add( GTK_CONTAINER( frame) , table );


#ifdef DEBUG  
  label = gtk_label_new_with_mnemonic ("ID:");
  gtk_table_attach ( GTK_TABLE( table ), label, 0, 1, 0, 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
  gtk_misc_set_alignment(GTK_MISC (label), 0, 0.5);
  entryID = gtk_entry_new();
  gtk_label_set_mnemonic_widget (GTK_LABEL (label), entryID);
  gtk_entry_set_text(GTK_ENTRY(entryID), curAccountID);
  gtk_widget_set_sensitive( GTK_WIDGET(entryID), FALSE);
  gtk_table_attach ( GTK_TABLE( table ), entryID, 1, 2, 0, 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
#endif 

  entryEnabled = gtk_check_button_new_with_mnemonic(_("_Enabled"));
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(entryEnabled), 
      g_strcasecmp(curAccountEnabled,"TRUE") == 0 ? TRUE: FALSE); 
  gtk_table_attach ( GTK_TABLE( table ), entryEnabled, 0, 2, 1, 2, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
  gtk_widget_set_sensitive( GTK_WIDGET( entryEnabled ) , TRUE );

  label = gtk_label_new_with_mnemonic (_("_Alias"));
  gtk_table_attach ( GTK_TABLE( table ), label, 0, 1, 3, 4, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
  gtk_misc_set_alignment(GTK_MISC (label), 0, 0.5);
  entryAlias = gtk_entry_new();
  gtk_label_set_mnemonic_widget (GTK_LABEL (label), entryAlias);
  gtk_entry_set_text(GTK_ENTRY(entryAlias), g_hash_table_lookup(currentAccount->properties, ACCOUNT_ALIAS));
  gtk_table_attach ( GTK_TABLE( table ), entryAlias, 1, 2, 3, 4, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

  label = gtk_label_new_with_mnemonic (_("_Protocol"));
  gtk_table_attach ( GTK_TABLE( table ), label, 0, 1, 4, 5, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
  gtk_misc_set_alignment(GTK_MISC (label), 0, 0.5);
  entryProtocol = gtk_combo_box_new_text();
  gtk_label_set_mnemonic_widget (GTK_LABEL (label), entryProtocol);
  gtk_combo_box_append_text(GTK_COMBO_BOX(entryProtocol), "SIP");
  if( is_iax_enabled() ) gtk_combo_box_append_text(GTK_COMBO_BOX(entryProtocol), "IAX");
  if(strcmp(curAccountType, "SIP") == 0)
  {
    gtk_combo_box_set_active(GTK_COMBO_BOX(entryProtocol),0);
  }
  else if(strcmp(curAccountType, "IAX") == 0)
  {
    gtk_combo_box_set_active(GTK_COMBO_BOX(entryProtocol),1);
  }
  else
  {
    /* Should never come here, add debug message. */
    gtk_combo_box_append_text(GTK_COMBO_BOX(entryProtocol), _("Unknown"));
    gtk_combo_box_set_active(GTK_COMBO_BOX(entryProtocol),2);  
  }
  gtk_table_attach ( GTK_TABLE( table ), entryProtocol, 1, 2, 4, 5, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

  /* Link signal 'changed' */
  g_signal_connect (G_OBJECT (GTK_COMBO_BOX(entryProtocol)), "changed",
      G_CALLBACK (change_protocol),
      currentAccount);

  label = gtk_label_new_with_mnemonic (_("_Host name"));
  gtk_table_attach ( GTK_TABLE( table ), label, 0, 1, 5, 6, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
  gtk_misc_set_alignment(GTK_MISC (label), 0, 0.5);
  entryHostname = gtk_entry_new();
  gtk_label_set_mnemonic_widget (GTK_LABEL (label), entryHostname);
  gtk_entry_set_text(GTK_ENTRY(entryHostname), curHostname);
  gtk_table_attach ( GTK_TABLE( table ), entryHostname, 1, 2, 5, 6, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

  label = gtk_label_new_with_mnemonic (_("_Port"));
  gtk_table_attach ( GTK_TABLE( table ), label, 0, 1, 6, 7, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
  gtk_misc_set_alignment(GTK_MISC (label), 0, 0.5);
  entryPort = gtk_entry_new();
  gtk_label_set_mnemonic_widget (GTK_LABEL (label), entryPort);
  gtk_entry_set_text(GTK_ENTRY(entryPort), curPort);
  gtk_table_attach ( GTK_TABLE( table ), entryPort, 1, 2, 6, 7, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

  label = gtk_label_new_with_mnemonic (_("_User name"));
  gtk_table_attach ( GTK_TABLE( table ), label, 0, 1, 7, 8, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
  gtk_misc_set_alignment(GTK_MISC (label), 0, 0.5);
  entryUsername = sexy_icon_entry_new();
  //image = gtk_image_new_from_stock( GTK_STOCK_DIALOG_AUTHENTICATION , GTK_ICON_SIZE_SMALL_TOOLBAR );
  image = gtk_image_new_from_file( ICONS_DIR "/stock_person.svg" );
  sexy_icon_entry_set_icon( SEXY_ICON_ENTRY(entryUsername), SEXY_ICON_ENTRY_PRIMARY , GTK_IMAGE(image) ); 
  gtk_label_set_mnemonic_widget (GTK_LABEL (label), entryUsername);
  gtk_entry_set_text(GTK_ENTRY(entryUsername), curUsername);
  gtk_table_attach ( GTK_TABLE( table ), entryUsername, 1, 2, 7, 8, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

  label = gtk_label_new_with_mnemonic (_("_Password"));
  gtk_table_attach ( GTK_TABLE( table ), label, 0, 1, 8, 9, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
  gtk_misc_set_alignment(GTK_MISC (label), 0, 0.5);
  entryPassword = sexy_icon_entry_new();
  image = gtk_image_new_from_stock( GTK_STOCK_DIALOG_AUTHENTICATION , GTK_ICON_SIZE_SMALL_TOOLBAR );
  sexy_icon_entry_set_icon( SEXY_ICON_ENTRY(entryPassword), SEXY_ICON_ENTRY_PRIMARY , GTK_IMAGE(image) ); 
  gtk_entry_set_visibility(GTK_ENTRY(entryPassword), FALSE);
  gtk_label_set_mnemonic_widget (GTK_LABEL (label), entryPassword);
  gtk_entry_set_text(GTK_ENTRY(entryPassword), curPassword);
  gtk_table_attach ( GTK_TABLE( table ), entryPassword, 1, 2, 8, 9, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

  label = gtk_label_new_with_mnemonic (_("_Voicemail box #"));
  gtk_table_attach ( GTK_TABLE( table ), label, 0, 1, 9, 10, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
  gtk_misc_set_alignment(GTK_MISC (label), 0, 0.5);
  entryMailbox = gtk_entry_new();
  gtk_label_set_mnemonic_widget (GTK_LABEL (label), entryMailbox);
  gtk_entry_set_text(GTK_ENTRY(entryMailbox), curMailbox);
  gtk_table_attach ( GTK_TABLE( table ), entryMailbox, 1, 2, 9, 10, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

  gtk_widget_show_all( table );
  gtk_container_set_border_width (GTK_CONTAINER(table), 10);

  frameNat = gtk_frame_new( _("Network Address Translation") );
  gtk_box_pack_start(GTK_BOX(dialog->vbox), frameNat, FALSE, FALSE, 0);
  gtk_widget_show(frameNat);

  tableNat = gtk_table_new ( 2, 2  ,  FALSE/* homogeneous */);
  gtk_table_set_row_spacings( GTK_TABLE(tableNat), 10);
  gtk_table_set_col_spacings( GTK_TABLE(tableNat), 10);
  gtk_widget_show(tableNat);
  gtk_container_add( GTK_CONTAINER( frameNat) , tableNat );

  // NAT detection code section
  stunEnable = gtk_check_button_new_with_mnemonic(_("E_nable STUN"));
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(stunEnable), strcmp(stun_enabled,"TRUE") == 0 ? TRUE: FALSE);
  g_signal_connect( G_OBJECT (GTK_TOGGLE_BUTTON(stunEnable)) , "toggled" , G_CALLBACK( stun_state ), NULL);
#if GTK_CHECK_VERSION(2,12,0)
  gtk_widget_set_tooltip_text( GTK_WIDGET( stunEnable ) , _("Enable it if you are behind a firewall, then restart SFLphone"));
#endif
  gtk_table_attach ( GTK_TABLE( tableNat ), stunEnable, 0, 1, 0, 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

  label = gtk_label_new_with_mnemonic(_("_STUN Server"));
  gtk_table_attach( GTK_TABLE( tableNat ), label, 0, 1, 1, 2, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
  gtk_misc_set_alignment(GTK_MISC (label), 0, 0.5);
  stunServer = gtk_entry_new();
  gtk_label_set_mnemonic_widget (GTK_LABEL (label), stunServer);
  gtk_entry_set_text(GTK_ENTRY(stunServer), stun_server);
#if GTK_CHECK_VERSION(2,12,0)
  gtk_widget_set_tooltip_text( GTK_WIDGET( stunServer ) , _("Format: name.server:port"));
#endif
  gtk_table_attach ( GTK_TABLE( tableNat ), stunServer, 1, 2, 1, 2, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
  gtk_widget_set_sensitive( GTK_WIDGET( stunServer ), gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(stunEnable)));


  // Toggle enabled/disabled widgets
  if (strcmp(curAccountType, "SIP") == 0) {
    //gtk_widget_set_sesitive( GTK_WIDGET(entryUserPart), TRUE);<    
  }
  else if (strcmp(curAccountType, "IAX") == 0) {
    gtk_widget_set_sensitive( GTK_WIDGET(stunEnable), FALSE);
    gtk_widget_set_sensitive( GTK_WIDGET(stunServer), FALSE);
  }
  else {
    // Disable everything ! ouch!
    // Shouldn't get there.
  }



  gtk_widget_show_all( tableNat );
  gtk_container_set_border_width (GTK_CONTAINER(tableNat), 10);

  response = gtk_dialog_run (GTK_DIALOG (dialog));
  if(response == GTK_RESPONSE_ACCEPT)
  {
    gchar* proto = (gchar *)gtk_combo_box_get_active_text(GTK_COMBO_BOX(entryProtocol));

    g_hash_table_replace(currentAccount->properties, 
    	g_strdup(ACCOUNT_ENABLED), 
    	 g_strdup(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(entryEnabled)) ? "TRUE": "FALSE"));
    g_hash_table_replace(currentAccount->properties, 
	g_strdup(ACCOUNT_ALIAS), 
	g_strdup((gchar *)gtk_entry_get_text(GTK_ENTRY(entryAlias))));
    g_hash_table_replace(currentAccount->properties, 
	g_strdup(ACCOUNT_TYPE), 
	g_strdup(proto));


    if (strcmp(proto, "SIP") == 0) { 
      g_hash_table_replace(currentAccount->properties, 
	  g_strdup(ACCOUNT_SIP_HOST), 
	  g_strdup((gchar *)gtk_entry_get_text(GTK_ENTRY(entryHostname))));

      g_hash_table_replace(currentAccount->properties, 
	  g_strdup(ACCOUNT_SIP_PORT), 
	  g_strdup((gchar *)gtk_entry_get_text(GTK_ENTRY(entryPort))));
      
      g_hash_table_replace(currentAccount->properties, 
	  g_strdup(ACCOUNT_SIP_USER), 
	  g_strdup((gchar *)gtk_entry_get_text(GTK_ENTRY(entryUsername))));
      g_hash_table_replace(currentAccount->properties, 
	  g_strdup(ACCOUNT_SIP_PASSWORD), 
	  g_strdup((gchar *)gtk_entry_get_text(GTK_ENTRY(entryPassword))));
      g_hash_table_replace(currentAccount->properties, 
	  g_strdup(ACCOUNT_SIP_STUN_SERVER), 
	  g_strdup((gchar *)gtk_entry_get_text(GTK_ENTRY(stunServer))));
      g_hash_table_replace(currentAccount->properties, 
	g_strdup(ACCOUNT_SIP_STUN_ENABLED), 
	g_strdup(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(stunEnable)) ? "TRUE": "FALSE"));
      g_hash_table_replace(currentAccount->properties, 
	g_strdup(ACCOUNT_MAILBOX), 
	g_strdup((gchar *)gtk_entry_get_text(GTK_ENTRY(entryMailbox))));
    }
    else if (strcmp(proto, "IAX") == 0) { 
      g_hash_table_replace(currentAccount->properties, 
	  g_strdup(ACCOUNT_IAX_HOST), 
	  g_strdup((gchar *)gtk_entry_get_text(GTK_ENTRY(entryHostname))));
      g_hash_table_replace(currentAccount->properties, 
	  g_strdup(ACCOUNT_IAX_USER), 
	  g_strdup((gchar *)gtk_entry_get_text(GTK_ENTRY(entryUsername))));
      g_hash_table_replace(currentAccount->properties, 
	  g_strdup(ACCOUNT_IAX_PASSWORD), 
	  g_strdup((gchar *)gtk_entry_get_text(GTK_ENTRY(entryPassword))));
      g_hash_table_replace(currentAccount->properties, 
	g_strdup(ACCOUNT_MAILBOX), 
	g_strdup((gchar *)gtk_entry_get_text(GTK_ENTRY(entryMailbox))));
    }
    else {

    }

    /** @todo Verify if it's the best condition to check */
    if (currentAccount->accountID == NULL) {
      dbus_add_account(currentAccount);
      account_list_set_current_id( currentAccount->accountID );
    }
    else {
      dbus_set_account_details(currentAccount);
      account_list_set_current_id( currentAccount->accountID);
    }
  }
  gtk_widget_destroy (GTK_WIDGET(dialog));
}
