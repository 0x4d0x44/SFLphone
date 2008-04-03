/*
 *  Copyright (C) 2008 Savoir-Faire Linux inc.
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

#ifndef PRESENCE_H
#define PRESENCE_H

#include <string>

/**
 * TOCOMMENT
 * @author Guillaume Carmel-Archambault
 */
class Presence {
	
public:
	Presence();
	Presence(std::string state, std::string additionalInfo);
	virtual ~Presence();
	
	std::string getState() { return _state; }
	std::string getAdditionalInfo() { return _additionalInfo; }
	
	void setState(std::string state) { _state = state; }
	void setAdditionalInfo(std::string additionalInfo) { _additionalInfo = additionalInfo; }
		
protected:
	
private:
	std::string _state;
	std::string _additionalInfo;
	std::string _capabalities;		// UNUSED Could be an independant attribute from presence included in a contact entry
	std::string _userAgent;			// UNUSED Name of user agent used by contact
};

#endif
