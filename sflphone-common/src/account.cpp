/*
 *  Copyright (C) 2006-2009 Savoir-Faire Linux inc.
 *
 *  Author: Emmanuel Milou <emmanuel.milou@savoirfairelinux.com>
 *  Author: Alexandre Bourget <alexandre.bourget@savoirfairelinux.com>
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

#include "account.h"
#include "manager.h"

Account::Account (const AccountID& accountID, std::string type) :
        _accountID (accountID)
        , _link (NULL)
        , _enabled (false)
        , _type (type)
		, _codecOrder ()
{
    setRegistrationState (Unregistered);
}

Account::~Account()
{
}

void Account::loadConfig() {

    std::string p;

    p =  Manager::instance().getConfigString (_accountID , CONFIG_ACCOUNT_TYPE);
#ifdef USE_IAX
    _enabled = (Manager::instance().getConfigString (_accountID, CONFIG_ACCOUNT_ENABLE) == "true") ? true : false;
#else

    if (p == "IAX")
        _enabled = false;
    else
        _enabled = (Manager::instance().getConfigString (_accountID, CONFIG_ACCOUNT_ENABLE) == "true") ? true : false;

#endif

	loadAudioCodecs ();
}

void Account::setRegistrationState (RegistrationState state) {

    if (state != _registrationState) {
        _debug ("Account::setRegistrationState");
        _registrationState = state;

        // Notify the client
        Manager::instance().connectionStatusNotification();
    }
}

void Account::loadAudioCodecs (void) {

	// if the user never set the codec list, use the default configuration for this account
    if (Manager::instance ().getConfigString (AUDIO, "ActiveCodecs") == "") {
		_warn ("use the default order");
		Manager::instance ().getCodecDescriptorMap ().setDefaultOrder();
    }

    // else retrieve the one set in the user config file
    else {
        std::vector<std::string> active_list = Manager::instance ().retrieveActiveCodecs();
		setActiveCodecs (active_list);
    }
}

void Account::setActiveCodecs (const std::vector <std::string> &list) {

    _codecOrder.clear();
    // list contains the ordered payload of active codecs picked by the user for this account
    // we used the CodecOrder vector to save the order.
    int i=0;
    int payload;
    size_t size = list.size();

		_warn ("set the custom order %i", list.size ());
	_warn ("Setting active codec list");

    while ( (unsigned int) i < size) {
        payload = std::atoi (list[i].data());
			_warn ("Adding codec with RTP payload=%i", payload);
        //if (Manager::instance ().getCodecDescriptorMap ().isCodecLoaded (payload)) {
            _codecOrder.push_back ( (AudioCodecType) payload);
        //}
        i++;
    }
}
