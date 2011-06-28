/*
 *  Copyright (C) 2004, 2005, 2006, 2009, 2008, 2009, 2010, 2011 Savoir-Faire Linux Inc.
 *  Author: Alexandre Savard <alexandre.savard@savoirfairelinux.com>
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

#ifndef __PREFERENCE_H__
#define __PREFERENCE_H__

#include "config/serializable.h"
#include "video/video_v4l2_list.h"
#include "video/video_v4l2.h"
using namespace sfl_video;

// general preferences
const Conf::Key orderKey ("order");                         // :	1234/2345/
const Conf::Key audioApiKey ("audioApi");                   // :	0
const Conf::Key historyLimitKey ("historyLimit");           // :	30
const Conf::Key historyMaxCallsKey ("historyMaxCalls");     // :	20
const Conf::Key notifyMailsKey ("notifyMails");             // :	false
const Conf::Key zoneToneChoiceKey ("zoneToneChoice");       // :	North America
const Conf::Key registrationExpireKey ("registrationExpire");// :	180
const Conf::Key portNumKey ("portNum");                     // :	5060
const Conf::Key searchBarDisplayKey ("searchBarDisplay");   // :	true
const Conf::Key zeroConfenableKey ("zeroConfenable");       // :	false
const Conf::Key md5HashKey ("md5Hash");                     // :	false

// voip preferences
const Conf::Key playDtmfKey ("playDtmf"); // true                    true
const Conf::Key playTonesKey ("playTones"); // true
const Conf::Key pulseLengthKey ("pulseLength"); //=250
const Conf::Key symmetricRtpKey ("symmetric");// =true
const Conf::Key zidFileKey ("zidFile");// =sfl.zid

// addressbook preferences
const Conf::Key photoKey ("photo");//		false
const Conf::Key enabledKey ("enabled");//		true
const Conf::Key listKey ("list");//		1243608768.30329.0@emilou-desktop/1243456917.15690.23@emilou-desktop/
const Conf::Key maxResultsKey ("maxResults");//		25
const Conf::Key businessKey ("business");//		true
const Conf::Key homeKey ("home");//		false
const Conf::Key mobileKey ("mobile");//		false

// hooks preferences
const Conf::Key iax2EnabledKey ("iax2Enabled");// :		false
const Conf::Key numberAddPrefixKey ("numberAddPrefix");//:	false
const Conf::Key numberEnabledKey ("numberEnabled"); //:	false
const Conf::Key sipEnabledKey ("sipEnabled"); //:		false
const Conf::Key urlCommandKey ("urlCommand"); //:		x-www-browser
const Conf::Key urlSipFieldKey ("urlSipField"); //:		X-sflphone-url

// audio preferences
const Conf::Key alsamapKey ("alsa");
const Conf::Key pulsemapKey ("pulse");
const Conf::Key cardinKey ("cardIn");// : 0
const Conf::Key cardoutKey ("cardOut");// 0
const Conf::Key cardringKey ("cardRing");// : 0
const Conf::Key framesizeKey ("frameSize");// : 20
const Conf::Key pluginKey ("plugin"); //: default
const Conf::Key smplrateKey ("smplRate");//: 44100
const Conf::Key devicePlaybackKey ("devicePlayback");//:
const Conf::Key deviceRecordKey ("deviceRecord");// :
const Conf::Key deviceRingtoneKey ("deviceRingtone");// :
const Conf::Key recordpathKey ("recordPath");//: /home/msavard/Bureau
const Conf::Key alwaysRecordingKey("alwaysRecording");
const Conf::Key volumemicKey ("volumeMic");//:  100
const Conf::Key volumespkrKey ("volumeSpkr");//: 100
const Conf::Key noiseReduceKey ("noiseReduce");
const Conf::Key echoCancelKey ("echoCancel");
const Conf::Key echoTailKey ("echoTailLength");
const Conf::Key echoDelayKey ("echoDelayLength");

// video preferences
const Conf::Key videoDeviceKey ("v4l2Dev");
const Conf::Key videoInputKey ("v4l2Input");
const Conf::Key videoSizeKey ("v4l2Size");
const Conf::Key videoRateKey ("v4l2Rate");

// shortcut preferences
const Conf::Key hangupShortKey ("hangUp");
const Conf::Key pickupShortKey ("pickUp");
const Conf::Key popupShortKey ("popupWindow");
const Conf::Key toggleHoldShortKey ("toggleHold");
const Conf::Key togglePickupHangupShortKey ("togglePickupHangup");


class Preferences : public Serializable
{

    public:

        Preferences();

        ~Preferences();

        virtual void serialize (Conf::YamlEmitter *emitter);

        virtual void unserialize (Conf::MappingNode *map);


        std::string getAccountOrder (void) {
            return _accountOrder;
        }
        void setAccountOrder (std::string ord) {
            _accountOrder = ord;
        }

        int getAudioApi (void) {
            return _audioApi;
        }
        void setAudioApi (int api) {
            _audioApi = api;
        }

        int getHistoryLimit (void) {
            return _historyLimit;
        }
        void setHistoryLimit (int lim) {
            _historyLimit = lim;
        }

        int getHistoryMaxCalls (void) {
            return _historyMaxCalls;
        }
        void setHistoryMaxCalls (int max) {
            _historyMaxCalls = max;
        }

        bool getNotifyMails (void) {
            return _notifyMails;
        }
        void setNotifyMails (bool mails) {
            _notifyMails = mails;
        }

        std::string getZoneToneChoice (void) {
            return _zoneToneChoice;
        }
        void setZoneToneChoice (std::string str) {
            _zoneToneChoice = str;
        }

        int getRegistrationExpire (void) {
            return _registrationExpire;
        }
        void setRegistrationExpire (int exp) {
            _registrationExpire = exp;
        }

        int getPortNum (void) {
            return _portNum;
        }
        void setPortNum (int port) {
            _portNum = port;
        }

        bool getSearchBarDisplay (void) {
            return _searchBarDisplay;
        }
        void setSearchBarDisplay (bool search) {
            _searchBarDisplay = search;
        }

        bool getZeroConfenable (void) {
            return _zeroConfenable;
        }
        void setZeroConfenable (bool enable) {
            _zeroConfenable = enable;
        }

        bool getMd5Hash (void) {
            return _md5Hash;
        }
        void setMd5Hash (bool md5) {
            _md5Hash = md5;
        }

    private:

        // account order
        std::string _accountOrder;

        int _audioApi;
        int _historyLimit;
        int _historyMaxCalls;
        bool _notifyMails;
        std::string _zoneToneChoice;
        int _registrationExpire;
        int _portNum;
        bool _searchBarDisplay;
        bool _zeroConfenable;
        bool _md5Hash;

};


class VoipPreference : public Serializable
{

    public:

        VoipPreference();

        ~VoipPreference();

        virtual void serialize (Conf::YamlEmitter *emitter);

        virtual void unserialize (Conf::MappingNode *map);

        bool getPlayDtmf (void) {
            return _playDtmf;
        }
        void setPlayDtmf (bool dtmf) {
            _playDtmf = dtmf;
        }

        bool getPlayTones (void) {
            return _playTones;
        }
        void setPlayTones (bool tone) {
            _playTones = tone;
        }

        int getPulseLength (void) {
            return _pulseLength;
        }
        void setPulseLength (int length) {
            _pulseLength = length;
        }

        bool getSymmetricRtp (void) {
            return _symmetricRtp;
        }
        void setSymmetricRtp (bool sym) {
            _symmetricRtp = sym;
        }

        std::string getZidFile (void) {
            return _zidFile;
        }
        void setZidFile (std::string file) {
            _zidFile = file;
        }

    private:

        bool _playDtmf;
        bool _playTones;
        int _pulseLength;
        bool _symmetricRtp;
        std::string _zidFile;

};

class AddressbookPreference : public Serializable
{

    public:

        AddressbookPreference();

        ~AddressbookPreference();

        virtual void serialize (Conf::YamlEmitter *emitter);

        virtual void unserialize (Conf::MappingNode *map);

        bool getPhoto (void) {
            return _photo;
        }
        void setPhoto (bool p) {
            _photo = p;
        }

        bool getEnabled (void) {
            return _enabled;
        }
        void setEnabled (bool e) {
            _enabled = e;
        }

        std::string getList (void) {
            return _list;
        }
        void setList (std::string l) {
            _list = l;
        }

        int getMaxResults (void) {
            return _maxResults;
        }
        void setMaxResults (int r) {
            _maxResults = r;
        }

        bool getBusiness (void) {
            return _business;
        }
        void setBusiness (bool b) {
            _business = b;
        }

        bool getHome (void) {
            return _home;
        }
        void setHone (bool h) {
            _home = h;
        }

        bool getMobile (void) {
            return _mobile;
        }
        void setMobile (bool m) {
            _mobile = m;
        }

    private:

        bool _photo;
        bool _enabled;
        std::string _list;
        int _maxResults;
        bool _business;
        bool _home;
        bool _mobile;

};


class HookPreference : public Serializable
{

    public:

        HookPreference();

        ~HookPreference();

        virtual void serialize (Conf::YamlEmitter *emitter);

        virtual void unserialize (Conf::MappingNode *map);

        bool getIax2Enabled (void) {
            return _iax2Enabled;
        }
        void setIax2Enabled (bool i) {
            _iax2Enabled = i;
        }

        std::string getNumberAddPrefix (void) {
            return _numberAddPrefix;
        }
        void setNumberAddPrefix (std::string n) {
            _numberAddPrefix = n;
        }

        bool getNumberEnabled (void) {
            return _numberEnabled;
        }
        void setNumberEnabled (bool n) {
            _numberEnabled = n;
        }

        bool getSipEnabled (void) {
            return _sipEnabled;
        }
        void setSipEnabled (bool s) {
            _sipEnabled = s;
        }

        std::string getUrlCommand (void) {
            return _urlCommand;
        }
        void setUrlCommand (std::string u) {
            _urlCommand = u;
        }

        std::string getUrlSipField (void) {
            return _urlSipField;
        }
        void setUrlSipField (std::string u) {
            _urlSipField = u;
        }

    private:

        bool _iax2Enabled;// :		false
        std::string _numberAddPrefix;//:	false
        bool _numberEnabled; //:	false
        bool _sipEnabled; //:		false
        std::string _urlCommand; //:		x-www-browser
        std::string _urlSipField; //:		X-sflphone-url

};


class AudioPreference : public Serializable
{

    public:

        AudioPreference();

        ~AudioPreference();

        virtual void serialize (Conf::YamlEmitter *emitter);

        virtual void unserialize (Conf::MappingNode *map);

        // alsa preference
        int getCardin (void) {
            return _cardin;
        }
        void setCardin (int c) {
            _cardin = c;
        }

        int getCardout (void) {
            return _cardout;
        }
        void setCardout (int c) {
            _cardout = c;
        }

        int getCardring (void) {
            return _cardring;
        }
        void setCardring (int c) {
            _cardring = c;
        }

        int getFramesize (void) {
            return _framesize;
        }
        void setFramesize (int f) {
            _framesize = f;
        }

        std::string getPlugin (void) {
            return _plugin;
        }
        void setPlugin (std::string p) {
            _plugin = p;
        }

        int getSmplrate (void) {
            return _smplrate;
        }
        void setSmplrate (int r) {
            _smplrate = r;
        }

        //pulseaudio preference
        std::string getDevicePlayback (void) {
            return _devicePlayback;
        }
        void setDevicePlayback (std::string p) {
            _devicePlayback = p;
        }

        std::string getDeviceRecord (void) {
            return _deviceRecord;
        }
        void setDeviceRecord (std::string r) {
            _deviceRecord = r;
        }

        std::string getDeviceRingtone (void) {
            return _deviceRingtone;
        }
        void setDeviceRingtone (std::string r) {
            _deviceRingtone = r;
        }

        // general preference
        std::string getRecordpath (void) {
            return _recordpath;
        }
        void setRecordpath (std::string r) {
            _recordpath = r;
        }

        bool getIsAlwaysRecording(void) {
        	return _alwaysRecording;
        }

        void setIsAlwaysRecording(bool rec) {
        	_alwaysRecording = rec;
        }

        int getVolumemic (void) {
            return _volumemic;
        }
        void setVolumemic (int m) {
            _volumemic = m;
        }

        int getVolumespkr (void) {
            return _volumespkr;
        }
        void setVolumespkr (int s) {
            _volumespkr = s;
        }

        bool getNoiseReduce (void) {
            return _noisereduce;
        }

        void setNoiseReduce (bool noise) {
            _noisereduce = noise;
        }

        bool getEchoCancel(void) {
        	return _echocancel;
        }

        void setEchoCancel(bool echo) {
        	_echocancel = echo;
        }

        int getEchoCancelTailLength(void) {
        	return _echoCancelTailLength;
        }

        void setEchoCancelTailLength(int length) {
        	_echoCancelTailLength = length;
        }

        int getEchoCancelDelay(void) {
        	return _echoCancelDelay;
        }

        void setEchoCancelDelay(int delay) {
        	_echoCancelDelay = delay;
        }

    private:

        // alsa preference
        int _cardin; // 0
        int _cardout; // 0
        int _cardring;// 0
        int _framesize; // 20
        std::string _plugin; // default
        int _smplrate;// 44100

        //pulseaudio preference
        std::string _devicePlayback;//:
        std::string _deviceRecord; //:
        std::string _deviceRingtone; //:

        // general preference
        std::string _recordpath; //: /home/msavard/Bureau
        bool _alwaysRecording;
        int _volumemic; //:  100
        int _volumespkr; //: 100

        bool _noisereduce;
        bool _echocancel;
        int _echoCancelTailLength;
        int _echoCancelDelay;
};


class VideoPreference : public Serializable
{

    public:

        VideoPreference();

        ~VideoPreference();

        virtual void serialize (Conf::YamlEmitter *emitter);

        virtual void unserialize (Conf::MappingNode *map);

        std::map<std::string, std::string> getVideoSettings(void) {
            std::map<std::string, std::string> map;
            VideoV4l2Device &dev = v4l2_list->getDevice();
            map["input"] = dev.device;
            std::stringstream channelstr;
            channelstr << dev.getChannelIndex();
            map["channel"] = channelstr.str();
            VideoV4l2Channel &chan = dev.getChannel();
            VideoV4l2Size &size = chan.getSize();
            std::stringstream ss;
            ss << size.width << "x" << size.height;
            map["video_size"] = ss.str();
            VideoV4l2Rate &rate = size.getRate();

            std::stringstream framestr;
            framestr << rate.num << "/" << rate.den;
            map["framerate"] = framestr.str();

            return map;
        }

        int getVideoDevice(void) {
            return _videoDevice;
        }

        void setVideoDevice(int device) {
            _videoDevice = device;
        }

        int getVideoInput(void) {
            return _videoInput;
        }

        void setVideoInput(int input) {
            _videoInput = input;
        }

        int getVideoSize(void) {
            return _videoSize;
        }

        void setVideoSize(int size) {
            _videoSize = size;
        }

        int getVideoRate(void) {
            return _videoRate;
        }

        void setVideoRate(int rate) {
            _videoRate = rate;
        }

        // V4L2 devices
        sfl_video::VideoV4l2List *v4l2_list;
    private:

        int _videoDevice;
        int _videoInput;
        int _videoSize;
        int _videoRate;
};


class ShortcutPreferences : public Serializable
{

    public:

        ShortcutPreferences();

        ~ShortcutPreferences();

        virtual void serialize (Conf::YamlEmitter *emitter);

        virtual void unserialize (Conf::MappingNode *map);

        void setShortcuts (std::map<std::string, std::string> shortcut);
        std::map<std::string, std::string> getShortcuts (void);

        std::string getHangup (void) {
            return _hangup;
        }
        void setHangup (std::string hangup) {
            _hangup = hangup;
        }

        std::string getPickup (void) {
            return _pickup;
        }
        void setPickup (std::string pickup) {
            _pickup = pickup;
        }

        std::string getPopup (void) {
            return _popup;
        }
        void setPopup (std::string popup) {
            _popup = popup;
        }

        std::string getToggleHold (void) {
            return _toggleHold;
        }
        void setToggleHold (std::string hold) {
            _toggleHold = hold;
        }

        std::string getTogglePickupHangup (void) {
            return _togglePickupHangup;
        }
        void setTogglePickupHangup (std::string toggle) {
            _togglePickupHangup = toggle;
        }

    private:

        std::string _hangup;
        std::string _pickup;
        std::string _popup;
        std::string _toggleHold;
        std::string _togglePickupHangup;

};

#endif
