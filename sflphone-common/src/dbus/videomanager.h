/*
 *  Copyright (C) 2010 Savoir-Faire Linux inc.
 *  Author: Pierre-Luc Bacon <pierre-luc.bacon@savoirfairelinux.com>
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

#ifndef _SFL_VIDEO_MANAGER_H_
#define _SFL_VIDEO_MANAGER_H_

#include "videomanager-glue.h"
#include <dbus-c++/dbus.h>

class VideoManager: public org::sflphone::SFLphone::VideoManager_adaptor,
		public DBus::IntrospectableAdaptor,
		public DBus::ObjectAdaptor {
public:

	VideoManager(DBus::Connection& connection);

	static const char* SERVER_PATH;

	/**
	 * @return A list containing the names of all the available capture devices.
	 */
	std::vector<std::string> enumerateDevices();

	/**
	 * @param device The device to start capturing from.
	 */
	void capture(const std::string& device);
};

#endif//CONTACTMANAGER_H
