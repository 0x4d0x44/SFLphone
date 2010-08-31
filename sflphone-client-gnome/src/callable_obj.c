/*
 *  Copyright (C) 2004, 2005, 2006, 2009, 2008, 2009, 2010 Savoir-Faire Linux Inc.
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
 *
 *  Additional permission under GNU GPL version 3 section 7:
 *
 *  If you modify this program, or any covered work, by linking or
 *  combining it with the OpenSSL project's OpenSSL library (or a
 *  modified version of that library), containing parts covered by the
 *  terms of the OpenSSL or SSLeay licenses, Savoir-Faire Linux Inc.
 *  grants you additional permission to convey the resulting work.
 *  Corresponding Source for a non-source form of such a combination
 *  shall include the source code for the parts of OpenSSL used as well
 *  as that of the covered work.
 */

#include <callable_obj.h>
#include <codeclist.h>
#include <sflphone_const.h>
#include <time.h>

#define UNIX_DAY			86400
#define UNIX_WEEK			86400 * 6
#define UNIX_TWO_DAYS		86400 * 2

gint is_callID_callstruct (gconstpointer a, gconstpointer b)
{
    callable_obj_t * c = (callable_obj_t*) a;

    if (g_strcasecmp (c->_callID, (const gchar*) b) == 0) {
        return 0;
    } else {
        return 1;
    }
}

gint get_state_callstruct (gconstpointer a, gconstpointer b)
{
    callable_obj_t * c = (callable_obj_t*) a;

    if (c->_state == * ( (call_state_t*) b)) {
        return 0;
    } else {
        return 1;
    }
}

gchar* call_get_peer_name (const gchar *format)
{
    const gchar *end, *name;

    DEBUG ("    callable_obj: %s", format);

    end = g_strrstr (format, "<");

    if (!end) {
        return g_strndup (format, 0);
    } else {
        name = format;
        return g_strndup (name, end - name);
    }
}

gchar* call_get_peer_number (const gchar *format)
{
    DEBUG ("    callable_obj: %s", format);

    gchar * number = g_strrstr (format, "<") + 1;
    gchar * end = g_strrstr (format, ">");

    if (end && number)
        number = g_strndup (number, end - number);
    else
        number = g_strdup (format);

    return number;
}

gchar* call_get_audio_codec (callable_obj_t *obj)
{
    gchar *audio_codec = "";
    codec_t *codec;
    gchar *format ="";
    int samplerate;

    if (obj) {
        audio_codec = dbus_get_current_codec_name (obj);
        codec = codec_list_get_by_name (audio_codec, NULL);

        if (codec) {
            samplerate = codec->sample_rate;
            format = g_markup_printf_escaped ("%s/%i", audio_codec, samplerate);
        }
    }

    return format;
}

void call_add_error (callable_obj_t * call, gpointer dialog)
{
    g_ptr_array_add (call->_error_dialogs, dialog);
}

void call_remove_error (callable_obj_t * call, gpointer dialog)
{
    g_ptr_array_remove (call->_error_dialogs, dialog);
}

void call_remove_all_errors (callable_obj_t * call)
{
    g_ptr_array_foreach (call->_error_dialogs, (GFunc) gtk_widget_destroy, NULL);
}

void create_new_call (callable_type_t type, call_state_t state, gchar* callID , gchar* accountID, gchar* peer_name, gchar* peer_number, callable_obj_t ** new_call)
{

    callable_obj_t *obj;
    gchar *call_id;

    // Allocate memory
    obj = g_new0 (callable_obj_t, 1);

    obj->_error_dialogs = g_ptr_array_new();

    // Set fields
    obj->_type = type;
    obj->_state = state;
    obj->_state_code = 0;
    obj->_state_code_description = "";
    obj->_accountID = g_strdup (accountID);
    obj->_peer_name = g_strdup (peer_name);
    obj->_peer_number = g_strdup (peer_number);
    obj->_peer_info = g_strdup (get_peer_info (peer_name, peer_number));

    obj->_trsft_to = "";
    set_timestamp (& (obj->_time_start));
    set_timestamp (& (obj->_time_stop));

    if (g_strcasecmp (callID, "") == 0)
        call_id = generate_call_id ();
    else
        call_id = callID;

    // Set the IDs
    obj->_callID = g_strdup (call_id);
    obj->_confID = NULL;

    *new_call = obj;
}

void create_new_call_from_details (const gchar *call_id, GHashTable *details, callable_obj_t **call)
{
    gchar *peer_name, *peer_number, *accountID, *state_str;
    callable_obj_t *new_call;
    call_state_t state;

    accountID = g_hash_table_lookup (details, "ACCOUNTID");
    peer_number = g_hash_table_lookup (details, "PEER_NUMBER");
    peer_name = g_hash_table_lookup (details, "DISPLAY_NAME");
    state_str = g_hash_table_lookup (details, "CALL_STATE");


    if (g_strcasecmp (state_str, "CURRENT") == 0)
        state = CALL_STATE_CURRENT;

    else if (g_strcasecmp (state_str, "RINGING") == 0)
        state = CALL_STATE_RINGING;

    else if (g_strcasecmp (state_str, "INCOMING") == 0)
        state = CALL_STATE_INCOMING;

    else if (g_strcasecmp (state_str, "HOLD") == 0)
        state = CALL_STATE_HOLD;

    else if (g_strcasecmp (state_str, "BUSY") == 0)
        state = CALL_STATE_BUSY;

    else
        state = CALL_STATE_FAILURE;

    create_new_call (CALL, state, (gchar*) call_id, accountID, peer_name, call_get_peer_number (peer_number), &new_call);
    *call = new_call;
}

void create_history_entry_from_serialized_form (gchar *timestamp, gchar *details, callable_obj_t **call)
{
    gchar *peer_name="";
    gchar *peer_number="", *accountID="", *time_stop="";
    callable_obj_t *new_call;
    history_state_t history_state = MISSED;
    char **ptr;
    const char *delim="|";
    int token=0;

    // details is in serialized form, i e: calltype%to%from%callid

    if ( (ptr = g_strsplit (details, delim,5)) != NULL) {

        while (ptr != NULL && token < 5) {

            switch (token) {
                case 0:
                    history_state = get_history_state_from_id (*ptr);
                    break;
                case 1:
                    peer_number = *ptr;
                    break;
                case 2:
                    peer_name = *ptr;
                    break;
                case 3:
                    time_stop = *ptr;
                    break;
                case 4:
                    accountID = *ptr;
                    break;
                default:
                    break;
            }

            token++;
            ptr++;

        }

    }

    if (g_strcasecmp (peer_name, "empty") == 0)
        peer_name="";

    create_new_call (HISTORY_ENTRY, CALL_STATE_DIALING, "", accountID, peer_name, peer_number, &new_call);
    new_call->_history_state = history_state;
    new_call->_time_start = convert_gchar_to_timestamp (timestamp);
    new_call->_time_stop = convert_gchar_to_timestamp (time_stop);

    *call = new_call;
}

void free_callable_obj_t (callable_obj_t *c)
{
    g_free (c->_callID);
    g_free (c->_accountID);
    g_free (c->_peer_name);
    g_free (c->_peer_number);
    g_free (c->_peer_info);
    g_free (c);
}

void attach_thumbnail (callable_obj_t *call, GdkPixbuf *pixbuf)
{
    call->_contact_thumbnail = pixbuf;
}

gchar* generate_call_id (void)
{
    gchar *call_id;

    call_id = g_new0 (gchar, 30);
    g_sprintf (call_id, "%d", rand());
    return call_id;
}

gchar* get_peer_info (gchar* number, gchar* name)
{
    gchar *info;

    info = g_strconcat ("\"", name, "\" <", number, ">", NULL);
    return info;
}

history_state_t get_history_state_from_id (gchar *indice)
{

    history_state_t state;

    if (g_strcasecmp (indice, "0") ==0)
        state = MISSED;
    else if (g_strcasecmp (indice, "1") ==0)
        state = INCOMING;
    else if (g_strcasecmp (indice, "2") ==0)
        state = OUTGOING;
    else
        state = MISSED;

    return state;
}

gchar* get_call_duration (callable_obj_t *obj)
{

    gchar *res;
    int duration;
    time_t start, end;

    start = obj->_time_start;
    end = obj->_time_stop;

    if (start == end)
        return g_markup_printf_escaped ("<small>Duration:</small> 0:00");

    duration = (int) difftime (end, start);

    if (duration / 60 == 0) {
        if (duration < 10)
            res = g_markup_printf_escaped ("00:0%i", duration);
        else
            res = g_markup_printf_escaped ("00:%i", duration);
    } else {
        if (duration%60 < 10)
            res = g_markup_printf_escaped ("%i:0%i" , duration/60 , duration%60);
        else
            res = g_markup_printf_escaped ("%i:%i" , duration/60 , duration%60);
    }

    return g_markup_printf_escaped ("<small>Duration:</small> %s", res);

}

gchar* serialize_history_entry (callable_obj_t *entry)
{
    // "0|514-276-5468|Savoir-faire Linux|144562458" for instance

    gchar* result;
    gchar* separator = "|";
    gchar* history_state, *timestamp;

    // Need the string form for the history state
    history_state = get_history_id_from_state (entry->_history_state);
    // and the timestamps
    timestamp = convert_timestamp_to_gchar (entry->_time_stop);

    result = g_strconcat (history_state, separator,
                          entry->_peer_number, separator,
                          g_strcasecmp (entry->_peer_name,"") ==0 ? "empty": entry->_peer_name, separator,
                          timestamp, separator,
                          g_strcasecmp (entry->_accountID,"") ==0 ? "empty": entry->_accountID,
                          NULL);

    return result;
}

gchar* get_history_id_from_state (history_state_t state)
{
    // Refer to history_state_t enum in callable_obj.h
    switch (state) {
        case MISSED:
            return "0";
        case INCOMING:
            return "1";
        case OUTGOING:
            return "2";
        default:
            return "0";
    }
}

gchar* get_formatted_start_timestamp (callable_obj_t *obj)
{

    struct tm* ptr;
    time_t lt, now;
    unsigned char str[100];

    if (obj) {
        // Fetch the current timestamp
        (void) time (&now);
        lt = obj->_time_start;

        ptr = localtime (&lt);

        if (now - lt < UNIX_WEEK) {
            if (now-lt < UNIX_DAY) {
                strftime ( (char *) str, 100, N_ ("today at %R"), (const struct tm *) ptr);
            } else {
                if (now - lt < UNIX_TWO_DAYS) {
                    strftime ( (char *) str, 100, N_ ("yesterday at %R"), (const struct tm *) ptr);
                } else {
                    strftime ( (char *) str, 100, N_ ("%A at %R"), (const struct tm *) ptr);
                }
            }
        } else {
            strftime ( (char *) str, 100, N_ ("%x at %R"), (const struct tm *) ptr);
        }

        // result function of the current locale
        return g_markup_printf_escaped ("\n%s\n" , str);
    }

    return "";
}

void set_timestamp (time_t *timestamp)
{
    time_t tmp;

    // Set to the current value
    (void) time (&tmp);
    *timestamp=tmp;
}

gchar* convert_timestamp_to_gchar (time_t timestamp)
{
    return g_markup_printf_escaped ("%i", (int) timestamp);
}

time_t convert_gchar_to_timestamp (gchar *timestamp)
{
    return (time_t) atoi (timestamp);
}

gchar*
get_peer_information (callable_obj_t *c)
{

    gchar *res;

    if (g_strcasecmp (c->_peer_name,"") == 0)
        return g_strdup (c->_peer_number);
    else
        return g_strdup (c->_peer_name);
}


