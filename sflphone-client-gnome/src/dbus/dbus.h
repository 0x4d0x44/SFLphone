/*
 *  Copyright (C) 2007 Savoir-Faire Linux inc.
 *  Author: Pierre-Luc Beaudoin <pierre-luc.beaudoin@savoirfairelinux.com>
 *  Author: Emmanuel Milou <emmanuel.milou@savoirfairelinux.com>
 *  Author: Guillaume Carmel-Archambault <guillaume.carmel-archambault@savoirfairelinux.com>
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


#ifndef __DBUS_H__
#define __DBUS_H__

#include <dbus/dbus-glib.h>

#include <accountlist.h>
#include <calllist.h>
#include <conferencelist.h>
#include <conference_obj.h>
#include <sflnotify.h>

/** @file dbus.h
  * @brief General DBus functions wrappers.
  */

/**
 * Try to connect to DBus services
 * @return TRUE if connection succeeded, FALSE otherwise
 */
gboolean dbus_connect ();

/**
 * Unreferences the proxies
 */
void dbus_clean ();

/**
 * CallManager - Hold a call
 * @param c The call to hold
 */
void dbus_hold (const callable_obj_t * c );

/**
 * CallManager - Unhold a call
 * @param c The call to unhold
 */
void dbus_unhold (const callable_obj_t * c );

/**
 * CallManager - Hang up a call
 * @param c The call to hang up
 */
void dbus_hang_up (const callable_obj_t * c );

/**
 * CallManager - Transfer a call
 * @param c The call to transfer
 */
void dbus_transfert (const callable_obj_t * c);

/**
 * CallManager - Accept a call
 * @param c The call to accept
 */
void dbus_accept (const callable_obj_t * c);

/**
 * CallManager - Refuse a call
 * @param c The call to refuse
 */
void dbus_refuse (const callable_obj_t * c);

/**
 * CallManager - Place a call
 * @param c The call to place
 */
void dbus_place_call (const callable_obj_t * c);


/**
 * ConfigurationManager - Get the list of the setup accounts
 * @return gchar** The list of accounts
 */
gchar ** dbus_account_list();

/**
 * ConfigurationManager - Get the details of a specific account
 * @param accountID The unique of the account
 * @return GHashTable* The details of the account
 */
GHashTable * dbus_account_details(gchar * accountID);

/**
 * ConfigurationManager - Set the details of a specific account
 * @param a The account to update
 */
void dbus_set_account_details(account_t *a);

/**
 * ConfigurationManager - Set the additional credential information 
 * of a specific account, for a specific credential index.
 * This function will add the new section on the server side
 * if it cannot be found.
 * @param a The account to update
 * @param index The index for the credential to update
 */
void dbus_set_credential(account_t *a, int index);

/**
 * ConfigurationManager - Set the additional credential information 
 * of a specific account, for a specific credential index.
 * This function will add the new section on the server side
 * if it cannot be found.
 * @param a The account to update
 * @return int The number of credentials specified
 */
int dbus_get_number_of_credential(gchar * accountID);

/**
 * ConfigurationManager - Delete all credentials defined for
 * a given account.
 * @param a The account id
 */
void dbus_delete_all_credential(account_t *a);

/**
 * ConfigurationManager - Set the number of credential that
 * is being used.
 * @param a The account id
 */
void dbus_set_number_of_credential(account_t *a, int number);

/**
 * ConfigurationManager - Set the additional credential information 
 * of a specific account, for a specific credential index.
 * This function will add the new section on the server side
 * if it cannot be found.
 * @param a The account to update
 * @param index The credential index
 * @return GHashTable* The credential at index "index" for the given account
 */
GHashTable* dbus_get_credential(gchar * accountID, int index);

/**
 * ConfigurationManager - Get the details for the ip2ip profile 
 */
GHashTable * dbus_get_ip2_ip_details(void);

/**
 * ConfigurationManager - Set the details for the ip2ip profile 
 */
void dbus_set_ip2ip_details(GHashTable * properties);

/**
 * ConfigurationManager - Send registration request
 * @param accountID The account to register/unregister
 * @param enable The flag for the type of registration
 *		 0 for unregistration request
 *		 1 for registration request
 */
void dbus_send_register( gchar* accountID , const guint enable );

/**
 * ConfigurationManager - Add an account to the list
 * @param a The account to add
 */
gchar* dbus_add_account(account_t *a);

/**
 * ConfigurationManager - Remove an account from the list
 * @param accountID The account to remove
 */
void dbus_remove_account(gchar * accountID);

/**
 * ConfigurationManager - Set volume for speaker/mic
 * @param device The speaker or the mic
 * @param value The new value
 */
void dbus_set_volume(const gchar * device, gdouble value);

/**
 * ConfigurationManager - Get the volume of a device
 * @param device The speaker or the mic
 */
gdouble dbus_get_volume(const gchar * device);

/**
 * ConfigurationManager - Play DTMF
 * @param key The DTMF to send
 */
void dbus_play_dtmf(const gchar * key);

/**
 * ConfigurationManager - Get the codecs list
 * @return gchar** The list of codecs
 */
gchar** dbus_codec_list();

/**
 * ConfigurationManager - Get the codec details
 * @param payload The payload of the codec
 * @return gchar** The codec details
 */
gchar** dbus_codec_details(int payload);

/**
 * ConfigurationManager - Get the default codec list
 * The default codec list are the codecs selected by the server if the user hasn't made any changes
 * @return gchar** The default codec list
 */
gchar** dbus_default_codec_list();

/**
 * ConfigurationManager - Get the list of the codecs used for media negociation
 * @return gchar** The list of codecs
 */
gchar** dbus_get_active_codec_list (gchar *accountID);

/**
 * ConfigurationManager - Set the list of codecs used for media negociation
 * @param list The list of codecs
 */
void dbus_set_active_codec_list (const gchar** list, const gchar*);

/**
 * CallManager - return the codec name
 * @param callable_obj_t* current call
 */
gchar* dbus_get_current_codec_name(const callable_obj_t * c);

/**
 * ConfigurationManager - Get the list of available input audio plugins
 * @return gchar** The list of plugins
 */
gchar** dbus_get_input_audio_plugin_list();

/**
 * ConfigurationManager - Get the list of available output audio plugins
 * @return gchar** The list of plugins
 */
gchar** dbus_get_output_audio_plugin_list();

/**
 * ConfigurationManager - Select an input audio plugin
 * @param audioPlugin The string description of the plugin
 */
void dbus_set_input_audio_plugin(gchar* audioPlugin);

/**
 * ConfigurationManager - Select an output audio plugin
 * @param audioPlugin The string description of the plugin
 */
void dbus_set_output_audio_plugin(gchar* audioPlugin);

/**
 * ConfigurationManager - Get the list of available output audio devices
 * @return gchar** The list of devices
 */
gchar** dbus_get_audio_output_device_list();

/**
 * ConfigurationManager - Select an output audio device
 * @param index The index of the soundcard
 */
void dbus_set_audio_output_device(const int index);

/**
 * ConfigurationManager - Get the list of available input audio devices
 * @return gchar** The list of devices
 */
gchar** dbus_get_audio_input_device_list();

/**
 * ConfigurationManager - Select an input audio device
 * @param index The index of the soundcard
 */
void dbus_set_audio_input_device(const int index);

/**
 * ConfigurationManager - Get the current audio devices
 * @return gchar** The index of the current soundcard
 */
gchar** dbus_get_current_audio_devices_index();

/**
 * ConfigurationManager - Get the index of the specified audio device
 * @param name The string description of the audio device
 * @return int The index of the device
 */
int dbus_get_audio_device_index(const gchar* name);

/**
 * ConfigurationManager - Get the current output audio plugin
 * @return gchar* The current plugin
 *		  default
 *		  plughw
 *		  dmix
 */
gchar* dbus_get_current_audio_output_plugin();

/**
 * ConfigurationManager - Query to server to 
 * know if MD5 credential hashing is enabled.
 * @return True if enabled, false otherwise
 *
 */
gboolean dbus_is_md5_credential_hashing();

/**
 * ConfigurationManager - Set whether or not
 * the server should store credential as
 * a md5 hash.
 * @param enabled 
 */
void dbus_set_md5_credential_hashing(gboolean enabled);

/**
 * ConfigurationManager - Tells the GUI if IAX2 support is enabled
 * @return int 1 if IAX2 is enabled
 *	       0 otherwise
 */
int dbus_is_iax2_enabled( void );

/**
 * ConfigurationManager - Query the server about the ringtone option.
 * If ringtone is enabled, ringtone on incoming call use custom choice. If not, only standart tone.
 * @return int	1 if enabled
 *	        0 otherwise
 */
int dbus_is_ringtone_enabled( void );

/**
 * ConfigurationManager - Set the ringtone option
 * Inverse current value
 */
void dbus_ringtone_enabled( void );

/**
 * ConfigurationManager - Get the ringtone
 * @return gchar* The file name selected as a ringtone
 */
gchar* dbus_get_ringtone_choice( void );

/**
 * ConfigurationManager - Set a ringtone
 * @param tone The file name of the ringtone
 */
void dbus_set_ringtone_choice( const gchar* tone );

/**
 * ConfigurationManager - Set the dialpad visible or not
 */
void dbus_set_dialpad (gboolean display);

/**
 * ConfigurationManager - Tells if the user wants to display the dialpad or not
 * @return int 1 if dialpad has to be displayed
 *	       0 otherwise
 */
int dbus_get_dialpad( void );

/**
 * ConfigurationManager - Set the searchbar visible or not
 */
void dbus_set_searchbar(  );

/**
 * ConfigurationManager - Tells if the user wants to display the search bar or not
 * @return int 1 if the search bar has to be displayed
 *	       0 otherwise
 */
int dbus_get_searchbar( void );

/**
 * ConfigurationManager - Set the volume controls visible or not
 */
void dbus_set_volume_controls (gboolean display);

/**
 * ConfigurationManager - Tells if the user wants to display the volume controls or not
 * @return int 1 if the controls have to be displayed
 *	       0 otherwise
 */
int dbus_get_volume_controls( void );

/**
 * ConfigurationManager - Configure the start-up option
 * At startup, SFLphone can be displayed or start hidden in the system tray
 */
void dbus_start_hidden( void );

/**
 * ConfigurationManager - Gives the maximum number of days the user wants to have in the history
 * @return double The maximum number of days
 */
guint dbus_get_history_limit( void );

/**
 * ConfigurationManager - Gives the maximum number of days the user wants to have in the history
 */
void dbus_set_history_limit (const guint days);

void dbus_set_history_enabled (void);

gchar* dbus_get_history_enabled (void);

/**
 * ConfigurationManager - Configure the start-up option
 * @return int	1 if SFLphone should start in the system tray
 *	        0 otherwise
 */
int dbus_is_start_hidden( void );

/**
 * ConfigurationManager - Configure the popup behaviour
 * When SFLphone is in the system tray, you can configure when it popups
 * Never or only on incoming calls
 */
void dbus_switch_popup_mode( void );

/**
 * ConfigurationManager - Configure the popup behaviour
 * @return int	1 if it should popup on incoming calls
 *		0 if it should never popups
 */
int dbus_popup_mode( void );

/**
 * ConfigurationManager - Returns the selected audio manager
 * @return int	0	ALSA
 *		1	PULSEAUDIO
 */
int dbus_get_audio_manager( void );

/**
 * ConfigurationManager - Set the audio manager
 * @param api	0	ALSA
 *		1	PULSEAUDIO
 */
void dbus_set_audio_manager( int api );

/**
 * ConfigurationManager - Configure the notification level
 * @return int	0 disable
 *		1 enable
 */
guint dbus_get_notify( void );

/**
 * ConfigurationManager - Configure the notification level
 */
void dbus_set_notify( void );

/**
 * ConfigurationManager - Start a tone when a new call is open and no numbers have been dialed
 * @param start 1 to start
 *		0 to stop
 * @param type  TONE_WITH_MESSAGE
 *		TONE_WITHOUT_MESSAGE
 */
void dbus_start_tone(const int start , const guint type);

/**
 * Instance - Send registration request to dbus service.
 * Manage the instances of clients connected to the server
 * @param pid The pid of the processus client
 * @param name The string description of the client. Here : GTK+ Client
 */
void dbus_register( int pid, gchar * name);

/**
 * Instance - Send unregistration request to dbus services
 * @param pid The pid of the processus
 */
void dbus_unregister(int pid);

void dbus_set_sip_address(const gchar* address);

gint dbus_get_sip_address(void);

void dbus_add_participant(const gchar* callID, const gchar* confID);

void dbus_set_record (const gchar * id);

void dbus_set_record_path (const gchar *path);
gchar* dbus_get_record_path (void);

/**
 * Encapsulate all the address book-related configuration
 * Get the configuration
 */
GHashTable* dbus_get_addressbook_settings (void);

/**
 * Encapsulate all the address book-related configuration
 * Set the configuration
 */
void dbus_set_addressbook_settings (GHashTable *);

gchar** dbus_get_addressbook_list (void);

void dbus_set_addressbook_list (const gchar** list);

/**
 * Resolve the local address given an interface name
 */
gchar * dbus_get_address_from_interface_name(gchar* interface);

/**
 * Query the daemon to return a list of network interface (described as there IP address)
 */
gchar** dbus_get_all_ip_interface(void);

/**
 * Query the daemon to return a list of network interface (described as there name)
 */
gchar** dbus_get_all_ip_interface_by_name(void);

/**
 * Encapsulate all the url hook-related configuration
 * Get the configuration
 */
GHashTable* dbus_get_hook_settings (void);

/**
 * Encapsulate all the url hook-related configuration
 * Set the configuration
 */
void dbus_set_hook_settings (GHashTable *);


gboolean dbus_get_is_recording(const callable_obj_t *);

GHashTable* dbus_get_call_details (const gchar* callID);

gchar** dbus_get_call_list (void);

GHashTable* dbus_get_conference_details (const gchar* confID);

gchar** dbus_get_conference_list (void);

void dbus_set_accounts_order (const gchar* order);

GHashTable* dbus_get_history (void);

void dbus_set_history (GHashTable* entries);

void sflphone_display_transfer_status (const gchar* message);

/**
 * CallManager - Confirm Short Authentication String 
 * for a given callId
 * @param c The call to confirm SAS
 */
void dbus_confirm_sas (const callable_obj_t * c);

/**
 * CallManager - Reset Short Authentication String 
 * for a given callId
 * @param c The call to reset SAS
 */
void dbus_reset_sas (const callable_obj_t * c);

/**
 * CallManager - Request Go Clear in the ZRTP Protocol 
 * for a given callId
 * @param c The call that we want to go clear
 */
void dbus_request_go_clear (const callable_obj_t * c);

/**
 * CallManager - Accept Go Clear request from remote
 * for a given callId
 * @param c The call to confirm
 */
void dbus_set_confirm_go_clear (const callable_obj_t * c);

/**
 * CallManager - Get the list of supported TLS methods from
 * the server in textual form.  
 * @return an array of string representing supported methods
 */
gchar** dbus_get_supported_tls_method();

gchar** dbus_get_participant_list (const char * confID);

guint dbus_get_window_width (void);
guint dbus_get_window_height (void);
void dbus_set_window_height (const guint height);
void dbus_set_window_width (const guint width);
guint dbus_get_window_position_x (void);
guint dbus_get_window_position_y (void);
void dbus_set_window_position_x (const guint posx);
void dbus_set_window_position_y (const guint posy);

GHashTable* dbus_get_shortcuts(void);
void dbus_set_shortcuts(GHashTable * shortcuts);

void dbus_enable_status_icon (const gchar*);
gchar* dbus_is_status_icon_enabled (void);

/**
 * Retrieve a printable list of all the video capture devices that are available.
 * @return The list of all the video capture devices that are available.
 */
gchar** dbus_video_enumerate_devices();

/**
 * Start capturing frames from a specified video device and write to shared memory.
 * @param device The video device to start capturing from.
 * @return A path to the shared memory segment.
 */
gchar* dbus_video_start_local_capture(gchar * device);

#endif
