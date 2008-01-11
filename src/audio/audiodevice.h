/*
 *  Copyright (C) 2006 Savoir-Faire Linux inc.
 *  Author: Yan Morin <yan.morin@savoirfairelinux.com>
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

#ifndef _AUDIO_DEVICE_H
#define _AUDIO_DEVICE_H


#include <string>

#define AUDIODEVICERATE 8000

/**
 * Container device for attribute storage
 * Have almost only get/set method
 */
class AudioDevice {
public:
  AudioDevice(int id, const std::string& apiName, const std::string& name);
  ~AudioDevice();

  const static double DEFAULT_RATE;

  int getId() { return _id; }
  const std::string& getApiName() {return _apiName; }
  const std::string& getName() {return _name; }

  void setRate(double rate) { _rate = rate;}
  double getRate() { return _rate; }

private:

  /** Integer id of the device, can not be 0 */
  int _id;
  /** Host API Name, ex: OSS, ALSA */
  std::string _apiName;
  /** Name of the device */
  std::string _name;
  
  /** Default rate in Hz, like 8000.0, default is AudioDevice::DEFAULT_RATE */
  double _rate;
};

#endif // _AUDIO_DEVICE_H_

