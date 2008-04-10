/*
 *  Copyright (C) 2007 Savoir-Faire Linux inc.
 *  Author: Pierre-Luc Beaudoin <pierre-luc@squidy.info>
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
#include <global.h>
#include <callmanager.h>
#include "../manager.h"

const char* CallManager::SERVER_PATH = "/org/sflphone/SFLphone/CallManager";

CallManager::CallManager( DBus::Connection& connection )
: DBus::ObjectAdaptor(connection, SERVER_PATH)
{
}

void
CallManager::placeCall( const ::DBus::String& accountID, 
                        const ::DBus::String& callID,          
                        const ::DBus::String& to )
{
    _debug("CallManager::placeCall received\n");
    Manager::instance().outgoingCall(accountID, callID, to);
}

void
CallManager::refuse( const ::DBus::String& callID )
{
    _debug("CallManager::refuse received\n");
    Manager::instance().refuseCall(callID);
}

void
CallManager::accept( const ::DBus::String& callID )
{
    _debug("CallManager::accept received\n");
    Manager::instance().answerCall(callID);
}

void
CallManager::hangUp( const ::DBus::String& callID )
{
    _debug("CallManager::hangUp received\n");
    Manager::instance().hangupCall(callID);

}

void
CallManager::hold( const ::DBus::String& callID )
{
    _debug("CallManager::hold received\n");
    Manager::instance().onHoldCall(callID);
    
}

void
CallManager::unhold( const ::DBus::String& callID )
{
    _debug("CallManager::unhold received\n");
    Manager::instance().offHoldCall(callID);
}

void
CallManager::transfert( const ::DBus::String& callID, const ::DBus::String& to )
{
    _debug("CallManager::transfert received\n");
    Manager::instance().transferCall(callID, to);
}

void
CallManager::setVolume( const ::DBus::String& device, const ::DBus::Double & value )
{
    _debug("CallManager::setVolume received\n");
    if(device == "speaker")
    {
      Manager::instance().setSpkrVolume((int)(value*100.0));
    }
    else if (device == "mic")
    {
      Manager::instance().setMicVolume((int)(value*100.0));
    }
    volumeChanged(device, value);
}

::DBus::Double 
CallManager::getVolume( const ::DBus::String& device )
{
    _debug("CallManager::getVolume received\n");
    if(device == "speaker")
    {
      _debug("Current speaker = %d\n", Manager::instance().getSpkrVolume());
      return Manager::instance().getSpkrVolume()/100.0;
    }
    else if (device == "mic")
    {
      _debug("Current mic = %d\n", Manager::instance().getMicVolume());
      return Manager::instance().getMicVolume()/100.0;
    }
    return 0;
}

std::map< ::DBus::String, ::DBus::String > 
CallManager::getCallDetails( const ::DBus::String& callID )
{
    _debug("CallManager::getCallDetails received\n");
    std::map<std::string, std::string> a;
    return a;
}

::DBus::String 
CallManager::getCurrentCallID(  )
{
    _debug("CallManager::getCurrentCallID received\n");
    return Manager::instance().getCurrentCallId();
}

void 
CallManager::playDTMF( const ::DBus::String& key )
{
  Manager::instance().sendDtmf(Manager::instance().getCurrentCallId(), key.c_str()[0]);
}

::DBus::String
CallManager::getLocalSharedMemoryKey()
{
	_debug("CallManager::getLocalSharedMemoryKey() received\n");
    return Manager::instance().getLocalSharedMemoryKey();
}

::DBus::String
CallManager::getRemoteSharedMemoryKey()
{
	_debug("CallManager::getRemoteSharedMemoryKey() received\n");
    return Manager::instance().getRemoteSharedMemoryKey();
}

::DBus::Bool 
CallManager::inviteConference( const ::DBus::String& accountID, const ::DBus::String& callID, const ::DBus::String& to )
{
	_debug("CallManager::inviteConference received\n");
    return Manager::instance().inviteConference(accountID, callID, to);
}
    
::DBus::Bool 
CallManager::joinConference( const ::DBus::String& onHoldCallID, const ::DBus::String& newCallID )
{
	_debug("CallManager::joinConference received\n");
    return Manager::instance().joinConference(onHoldCallID, newCallID);
}


void 
CallManager::changeWebcamStatus( const ::DBus::Bool& status, const ::DBus::String& callID  )
{
	_debug("CallManager::changeWebcamStatus() received\n");
    Manager::instance().changeWebcamStatus(status, callID);
}


