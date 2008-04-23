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

#include <global.h>
#include <configurationmanager.h>
#include <sstream>
#include "../manager.h"

const char* ConfigurationManager::SERVER_PATH = "/org/sflphone/SFLphone/ConfigurationManager";



	ConfigurationManager::ConfigurationManager( DBus::Connection& connection )
: DBus::ObjectAdaptor(connection, SERVER_PATH)
{
}

	std::map< ::DBus::String, ::DBus::String > 
ConfigurationManager::getAccountDetails( const ::DBus::String& accountID )
{
	_debug("ConfigurationManager::getAccountDetails received\n");
	return Manager::instance().getAccountDetails(accountID);
}

	void 
ConfigurationManager::setAccountDetails( const ::DBus::String& accountID, 
		const std::map< ::DBus::String, ::DBus::String >& details )
{
	_debug("ConfigurationManager::setAccountDetails received\n");
	Manager::instance().setAccountDetails(accountID, details);
}

	void 
ConfigurationManager::sendRegister( const ::DBus::String& accountID, const ::DBus::Int32& expire )
{
	Manager::instance().sendRegister(accountID, expire);
}

	void 
ConfigurationManager::addAccount( const std::map< ::DBus::String, ::DBus::String >& details )
{
	_debug("ConfigurationManager::addAccount received\n");
	Manager::instance().addAccount(details);
}


	void 
ConfigurationManager::removeAccount( const ::DBus::String& accoundID )
{
	_debug("ConfigurationManager::removeAccount received\n");
	return Manager::instance().removeAccount(accoundID);
}

std::vector< ::DBus::String > 
ConfigurationManager::getAccountList(  )
{
	_debug("ConfigurationManager::getAccountList received\n");
	return Manager::instance().getAccountList();
}


std::vector< ::DBus::String > 
ConfigurationManager::getToneLocaleList(  )
{
	_debug("ConfigurationManager::getToneLocaleList received\n");

}



	::DBus::String 
ConfigurationManager::getVersion(  )
{
	_debug("ConfigurationManager::getVersion received\n");

}


	std::vector< ::DBus::String > 
ConfigurationManager::getRingtoneList(  )
{
	_debug("ConfigurationManager::getRingtoneList received\n");

}



	std::vector< ::DBus::String  > 
ConfigurationManager::getCodecList(  )
{
	_debug("ConfigurationManager::getCodecList received\n");
	return Manager::instance().getCodecList();
}

	std::vector< ::DBus::String > 
ConfigurationManager::getCodecDetails( const ::DBus::Int32& payload )
{
	_debug("ConfigurationManager::getCodecDetails received\n");
	return Manager::instance().getCodecDetails( payload );
}

	std::vector< ::DBus::String > 
ConfigurationManager::getActiveCodecList(  )
{
	_debug("ConfigurationManager::getActiveCodecList received\n");
	return Manager::instance().getActiveCodecList();
}

void 
ConfigurationManager::setActiveCodecList( const std::vector< ::DBus::String >& list )
{
	_debug("ConfigurationManager::setActiveCodecList received\n");
	 Manager::instance().setActiveCodecList(list);
}

	std::vector< ::DBus::String  > 
ConfigurationManager::getVideoCodecList(  )
{
	_debug("ConfigurationManager::getVideoCodecList received\n");
	return Manager::instance().getVideoCodecList();
}


	std::vector< ::DBus::String > 
ConfigurationManager::getActiveVideoCodecList(  )
{
	_debug("ConfigurationManager::getActiveVideoCodecList received\n");
	return Manager::instance().getActiveVideoCodecList();
}

void 
ConfigurationManager::setActiveVideoCodecList( const std::vector< ::DBus::String >& list )
{
	_debug("ConfigurationManager::setActiveVideoCodecList received\n");
	 Manager::instance().setActiveVideoCodecList(list);
}

// Audio devices related methods
  std::vector< ::DBus::String >
ConfigurationManager::getInputAudioPluginList()
{
	_debug("ConfigurationManager::getInputAudioPluginList received\n");
	return Manager::instance().getInputAudioPluginList();
}

  std::vector< ::DBus::String >
ConfigurationManager::getOutputAudioPluginList()
{
	_debug("ConfigurationManager::getOutputAudioPluginList received\n");
	return Manager::instance().getOutputAudioPluginList();
}

  void
ConfigurationManager::setInputAudioPlugin(const ::DBus::String& audioPlugin)
{
	_debug("ConfigurationManager::setInputAudioPlugin received\n");
	return Manager::instance().setInputAudioPlugin(audioPlugin);
}

  void
ConfigurationManager::setOutputAudioPlugin(const ::DBus::String& audioPlugin)
{
	_debug("ConfigurationManager::setOutputAudioPlugin received\n");
	return Manager::instance().setOutputAudioPlugin(audioPlugin);
}

  std::vector< ::DBus::String >
ConfigurationManager::getAudioOutputDeviceList()
{
	_debug("ConfigurationManager::getAudioOutputDeviceList received\n");
	return Manager::instance().getAudioOutputDeviceList();
}
void
ConfigurationManager::setAudioOutputDevice(const ::DBus::Int32& index)
{
	_debug("ConfigurationManager::setAudioOutputDevice received\n");
	return Manager::instance().setAudioOutputDevice(index);
}
std::vector< ::DBus::String >
ConfigurationManager::getAudioInputDeviceList()
{
	_debug("ConfigurationManager::getAudioInputDeviceList received\n");
	return Manager::instance().getAudioInputDeviceList();
}
void
ConfigurationManager::setAudioInputDevice(const ::DBus::Int32& index)
{
	_debug("ConfigurationManager::setAudioInputDevice received\n");
	return Manager::instance().setAudioInputDevice(index);
}
std::vector< ::DBus::String >
ConfigurationManager::getCurrentAudioDevicesIndex()
{
	_debug("ConfigurationManager::getCurrentAudioDeviceIndex received\n");
	return Manager::instance().getCurrentAudioDevicesIndex();
}
 ::DBus::Int32
ConfigurationManager::getAudioDeviceIndex(const ::DBus::String& name)
{
	_debug("ConfigurationManager::getAudioDeviceIndex received\n");
	return Manager::instance().getAudioDeviceIndex(name);
}

::DBus::String 
ConfigurationManager::getCurrentAudioOutputPlugin( void )
{
   _debug("ConfigurationManager::getCurrentAudioOutputPlugin received\n");
   return Manager::instance().getCurrentAudioOutputPlugin();
}


	std::vector< ::DBus::String > 
ConfigurationManager::getPlaybackDeviceList(  )
{
	_debug("ConfigurationManager::getPlaybackDeviceList received\n");

}

	std::vector< ::DBus::String > 
ConfigurationManager::getRecordDeviceList(  )
{
	_debug("ConfigurationManager::getRecordDeviceList received\n");

}

::DBus::Int32
ConfigurationManager::isIax2Enabled( void )
{
  return Manager::instance().isIax2Enabled(  ); 
}

void
ConfigurationManager::ringtoneEnabled( void )
{
  Manager::instance().ringtoneEnabled(  ); 
}

::DBus::Int32
ConfigurationManager::isRingtoneEnabled( void )
{
  return Manager::instance().isRingtoneEnabled(  ); 
}

::DBus::String
ConfigurationManager::getRingtoneChoice( void )
{
  return Manager::instance().getRingtoneChoice(  ); 
}

void
ConfigurationManager::setRingtoneChoice( const ::DBus::String& tone )
{
  Manager::instance().setRingtoneChoice( tone ); 
}

/* Webcam Settings */

void
ConfigurationManager::getBrightness( ::DBus::Int32& minValue, ::DBus::Int32& maxValue, 
    			::DBus::Int32& stepValue, ::DBus::Int32& currentValue )
{
	_debug("ConfigurationManager::getBrightness received\n");
	CmdDesc values;
	
	values = Manager::instance().getBrightness();
	minValue = values.Min;
	maxValue = values.Max;
	stepValue = values.Step;
	currentValue = values.Current;
	printf("%i %i %i %i", minValue, maxValue,stepValue, currentValue);
}

void 
ConfigurationManager::setBrightness( const ::DBus::Int32& value )
{
	 _debug("ConfigurationManager::setBrightness received\n");
	Manager::instance().setBrightness(value);
}

void 
ConfigurationManager::getContrast( ::DBus::Int32& minValue, ::DBus::Int32& maxValue, 
    			::DBus::Int32& stepValue, ::DBus::Int32& currentValue )
{
	_debug("ConfigurationManager::getContrast received\n");
	CmdDesc values;
	
	values = Manager::instance().getContrast();
	minValue = values.Min;
	maxValue = values.Max;
	stepValue = values.Step;
	currentValue = values.Current;
	printf("%i %i %i %i", minValue, maxValue,stepValue, currentValue);
}

::DBus::Int32
ConfigurationManager::getDialpad( void )
{
  return Manager::instance().getDialpad(  ); 
}

void
ConfigurationManager::setDialpad( void )
{
  Manager::instance().setDialpad( ); 
}

void
ConfigurationManager::startHidden( void )
{
  Manager::instance().startHidden(  ); 
}

::DBus::Int32
ConfigurationManager::isStartHidden( void )
{
  return Manager::instance().isStartHidden(  ); 
}

void
ConfigurationManager::switchPopupMode( void )
{
  Manager::instance().switchPopupMode();
}

::DBus::Int32
ConfigurationManager::popupMode( void )
{
  return Manager::instance().popupMode();
}

void 
ConfigurationManager::setContrast( const ::DBus::Int32& value )
{
	 _debug("ConfigurationManager::setContrast received\n");
	Manager::instance().setContrast(value);
}

void 
ConfigurationManager::getColour( ::DBus::Int32& minValue, ::DBus::Int32& maxValue, 
    			::DBus::Int32& stepValue, ::DBus::Int32& currentValue )
{
	_debug("ConfigurationManager::getColour received\n");
	CmdDesc values;
	
	values = Manager::instance().getColour();
	minValue = values.Min;
	maxValue = values.Max;
	stepValue = values.Step;
	currentValue = values.Current;
	printf("%i %i %i %i", minValue, maxValue,stepValue, currentValue);
}

void 
ConfigurationManager::setColour( const ::DBus::Int32& value )
{
	 _debug("ConfigurationManager::setColour received\n");
	Manager::instance().setColour(value);
}

std::vector< ::DBus::String > 
ConfigurationManager::getWebcamDeviceList(  )
{
	_debug("ConfigurationManager::getWebcamDeviceList received\n");
	return Manager::instance().getWebcamDeviceList();
}

void 
ConfigurationManager::setWebcamDevice( const ::DBus::String& name )
{
	_debug("ConfigurationManager::setWebcamDevice received\n");
	Manager::instance().setWebcamDevice(name);
}

std::vector< ::DBus::String > 
ConfigurationManager::getResolutionList(  )
{
	_debug("ConfigurationManager::getResolutionList received\n");
	return Manager::instance().getResolutionList();
}

void 
ConfigurationManager::setResolution( const ::DBus::String& name )
{
  _debug("ConfigurationManager::setResolution received\n");
  Manager::instance().setResolution(name);
}

void
ConfigurationManager::setNotify( void )
{
  _debug("Manager received setNotify\n");
  Manager::instance().setNotify();
}

::DBus::String 
ConfigurationManager::getCurrentResolution()
{
  _debug("ConfigurationManager::getCurrentResolution received\n");
  return Manager::instance().getCurrentResolution();
}

::DBus::Int32
ConfigurationManager::getNotify(void)
{
  _debug("Manager received getNotify\n");
  return Manager::instance().getNotify();
}

std::vector< ::DBus::String > 
ConfigurationManager::getBitrateList()
{
  _debug("ConfigurationManager::getBitrateList received\n");
  return Manager::instance().getBitrateList();
}

void 
ConfigurationManager::setBitrate( const ::DBus::String& name )
{
  _debug("ConfigurationManager::setBitrate received\n");
  Manager::instance().setBitrate(name);
}

::DBus::String 
ConfigurationManager::getCurrentBitrate()
{
  _debug("ConfigurationManager::getCurrentBitrate received\n");
  return Manager::instance().getCurrentBitrate();
}

::DBus::Bool 
ConfigurationManager::enableLocalVideoPref(  )
{
	_debug("ConfigurationManager::enableLocalVideoPref received\n");
	return Manager::instance().enableLocalVideoPref();
}

::DBus::Bool 
ConfigurationManager::disableLocalVideoPref(  )
{
	_debug("ConfigurationManager::disableLocalVideoPref received\n");
	return Manager::instance().disableLocalVideoPref();
}

::DBus::Bool 
ConfigurationManager::getEnableCheckboxStatus(  )
{
	_debug("ConfigurationManager::getEnableCheckboxStatus received\n");
	return Manager::instance().getEnableCheckboxStatus();
}

::DBus::Bool 
ConfigurationManager::getDisableCheckboxStatus(  )
{
	_debug("ConfigurationManager::getDisableCheckboxStatus received\n");
	return Manager::instance().getDisableCheckboxStatus();
}

void 
ConfigurationManager::setEnableCheckboxStatus( const ::DBus::Bool& status )
{
	_debug("ConfigurationManager::setEnableCheckboxStatus received\n");
	Manager::instance().setEnableCheckboxStatus(status);
}

void 
ConfigurationManager::setDisableCheckboxStatus( const ::DBus::Bool& status )
{
	_debug("ConfigurationManager::setDisableCheckboxStatus received\n");
	Manager::instance().setDisableCheckboxStatus(status);
}

void
ConfigurationManager::setMailNotify( void )
{
  _debug("Manager received setMailNotify\n");
  Manager::instance().setMailNotify( ); 
}

::DBus::Int32
ConfigurationManager::getMailNotify( void )
{
  _debug("Manager received getMailNotify\n");
  return Manager::instance().getMailNotify(  ); 
}
