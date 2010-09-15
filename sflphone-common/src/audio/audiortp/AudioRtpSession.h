/*
 *  Copyright (C) 2004, 2005, 2006, 2009, 2008, 2009, 2010 Savoir-Faire Linux Inc.
 *  Author: Pierre-Luc Bacon <pierre-luc.bacon@savoirfairelinux.com>
 *  Author: Alexandre Bourget <alexandre.bourget@savoirfairelinux.com>
 *  Author: Laurielle Lea <laurielle.lea@savoirfairelinux.com>
 *  Author: Emmanuel Milou <emmanuel.milou@savoirfairelinux.com>
 *  Author: Yan Morin <yan.morin@savoirfairelinux.com>
 *  Author: Alexandre Savard <alexandre.savard@savoirfairelinux.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
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
#ifndef __SFL_AUDIO_RTP_SESSION_H__
#define __SFL_AUDIO_RTP_SESSION_H__

#include <iostream>
#include <exception>
#include <list>

#include "global.h"

#include "sip/sipcall.h"
#include "sip/sdp.h"
#include "audio/audiolayer.h"
#include "audio/codecs/audiocodec.h"
#include "audio/samplerateconverter.h"
#include "managerimpl.h"

#include <ccrtp/rtp.h>
#include <cc++/numbers.h> // ost::Time

#include <speex/speex_jitter.h>

#include "audio/jitterbuf.h"

#include <fstream>

// Frequency (in packet number)
#define RTP_TIMESTAMP_RESET_FREQ 100

// Factor use to increase volume in fade in
#define FADEIN_STEP_SIZE 4;

namespace sfl
{

static const int schedulingTimeout = 100000;
static const int expireTimeout = 1000000;


class AudioRtpSessionException: public std::exception
{
        virtual const char* what() const throw() {
            return "AudioRtpSessionException occured";
        }
};

typedef struct DtmfEvent {
    ost::RTPPacket::RFC2833Payload payload;
    int length;
    bool newevent;
} DtmfEvent;

typedef list<DtmfEvent *> EventQueue;


template <typename D>
class AudioRtpSession : public ost::Thread, public ost::TimerPort
{
    public:
        /**
        * Constructor
        * @param sipcall The pointer on the SIP call
        */
        AudioRtpSession (ManagerImpl * manager, SIPCall* sipcall);

        ~AudioRtpSession();

        // Thread associated method
        virtual void run ();

        int startRtpThread (AudioCodec*);

        /**
         * Used mostly when receiving a reinvite
         */
        void updateDestinationIpAddress (void);

        void putDtmfEvent (int digit);

        /**
        * Send DTMF over RTP (RFC2833). The timestamp and sequence number must be
        * incremented as if it was microphone audio. This function change the payload type of the rtp session,
        * send the appropriate DTMF digit using this payload, discard coresponding data from mainbuffer and get
        * back the codec payload for further audio processing.
        */
        void sendDtmfEvent (sfl::DtmfEvent *dtmf);

        inline float computeCodecFrameSize (int codecSamplePerFrame, int codecClockRate) {
            return ( (float) codecSamplePerFrame * 1000.0) / (float) codecClockRate;
        }

        int computeNbByteAudioLayer (float codecFrameSize) {
            return (int) ( ( (float) _converterSamplingRate * codecFrameSize * sizeof (SFLDataFormat)) / 1000.0);
        }

    private:

        /**
         * Allocate memory for RTP buffers and fill them with zeros
         */
        void initBuffers (void);

        /**
         * Set RTP Sockets send/receive timeouts
         */
        void setSessionTimeouts (void);

        /**
         * Set the audio codec for this RTP session
         */
        void setSessionMedia (AudioCodec*);

        /**
         * Retreive destination address for this session. Stored in CALL
         */
        void setDestinationIpAddress (void);

        /**
         * Encode audio data from mainbuffer
         */
        int processDataEncode (void);

        /**
         * Decode audio data received from peer
         */
        void processDataDecode (unsigned char * spkrData, unsigned int size);

        /**
         * Send encoded data to peer
         */
        void sendMicData();

        /**
         * Receive data from peer
         */
        void receiveSpeakerData ();

        /**
        * Ramp In audio data to avoid audio click from peer
        */
        bool fadeIn (SFLDataFormat *audio, int size, SFLDataFormat *factor);

        ost::Time * _time;

        // This semaphore is not used
        // but is needed in order to avoid
        // ambiguous compiling problem.
        // It is set to 0, and since it is
        // optional in ost::thread, then
        // it amounts to the same as doing
        // start() with no semaphore at all.
        ost::Semaphore * _mainloopSemaphore;

        // Main destination address for this rtp session.
        // Stored in case or reINVITE, which may require to forget
        // this destination and update a new one.
        ost::InetHostAddress _remote_ip;


        // Main destination port for this rtp session.
        // Stored in case reINVITE, which may require to forget
        // this destination and update a new one
        unsigned short _remote_port;

        AudioCodec * _audiocodec;

        AudioLayer * _audiolayer;

        /** Mic-data related buffers */
        SFLDataFormat* _micData;
        SFLDataFormat* _micDataConverted;
        unsigned char* _micDataEncoded;

        /** Speaker-data related buffers */
        SFLDataFormat* _spkrDataDecoded;
        SFLDataFormat* _spkrDataConverted;

        /** Sample rate converter object */
        SamplerateConverter * _converter;

        /** Variables to process audio stream: sample rate for playing sound (typically 44100HZ) */
        int _layerSampleRate;

        /** Sample rate of the codec we use to encode and decode (most of time 8000HZ) */
        int _codecSampleRate;

        /** Length of the sound frame we capture in ms (typically 20ms) */
        int _layerFrameSize;

        /** Codecs frame size in samples (20 ms => 882 at 44.1kHz)
            The exact value is stored in the codec */
        int _codecFrameSize;

        /** Speaker buffer length in samples once the data are resampled
         *  (used for mixing and recording)
         */
        int _nSamplesSpkr;

        /** Mic buffer length in samples once the data are resampled
         *  (used for mixing and recording)
         */
        int _nSamplesMic;

        /**
         * Maximum number of sample for audio buffers (mic and spkr)
         */
        int _nbSamplesMax;

        /**
         * Manager instance.
         */
        ManagerImpl * _manager;

        /**
         * Sampling rate of audio converter
         */
        int _converterSamplingRate;

        /**
         * Timestamp for this session
         */
        int _timestamp;

        /**
         * Timestamp incrementation value based on codec period length (framesize)
         * except for G722 which require a 8 kHz incrementation.
         */
        int _timestampIncrement;

        /**
         * Timestamp reset freqeuncy specified in number of packet sent
         */
        short _timestampCount;

        /**
         * Time counter used to trigger incoming call notification
         */
        int _countNotificationTime;

        /**
         * EventQueue used to store list of DTMF-
         */
        EventQueue _eventQueue;

        /**
         * Packet size in ms
         */
        int _packetLength;

        int _ts;

        /**
         * Current time in ms
         */
        int _currentTime;

        /**
         * Preprocess internal data
         */
        SpeexPreprocessState *_noiseState;

        /**
         * State of mic fade in
         */
        bool _micFadeInComplete;

        /**
           * State of spkr fade in
         */
        bool _spkrFadeInComplete;

        /**
         * Ampliturde factor to fade in mic data
         */
        SFLDataFormat _micAmplFactor;

        /**
         * Amplitude factor to fade in spkr data
         */
        SFLDataFormat _spkrAmplFactor;


    protected:

        SIPCall * _ca;

        bool onRTPPacketRecv (ost::IncomingRTPPkt&);
};

template <typename D>
AudioRtpSession<D>::AudioRtpSession (ManagerImpl * manager, SIPCall * sipcall) :
        _time (new ost::Time()),
        _mainloopSemaphore (0),
        _audiocodec (NULL),
        _audiolayer (NULL),
        _micData (NULL),
        _micDataConverted (NULL),
        _micDataEncoded (NULL),
        _spkrDataDecoded (NULL),
        _spkrDataConverted (NULL),
        _converter (NULL),
        _layerSampleRate (0),
        _codecSampleRate (0),
        _layerFrameSize (0),
        _manager (manager),
        _converterSamplingRate (0),
        _timestamp (0),
        _timestampIncrement (0),
        _timestampCount (0),
        _countNotificationTime (0),
        _noiseState (NULL),
        _micFadeInComplete (false),
        _spkrFadeInComplete (false),
        _micAmplFactor (32000),
        _spkrAmplFactor (32000),
        _ca (sipcall)
{
    setCancel (cancelDefault);

    assert (_ca);

    _info ("Rtp: Local audio port %i will be used", _ca->getLocalAudioPort());

    //mic, we receive from soundcard in stereo, and we send encoded
    _audiolayer = _manager->getAudioDriver();

    if (_audiolayer == NULL) {
        throw AudioRtpSessionException();
    }

    _layerFrameSize = _audiolayer->getFrameSize(); // in ms
    _layerSampleRate = _audiolayer->getSampleRate();

    _ts= 0;
    _packetLength = 20;
    _currentTime = 0;

}

template <typename D>
AudioRtpSession<D>::~AudioRtpSession()
{
    _debug ("RTP: Delete AudioRtpSession instance");

    try {
        terminate();
    } catch (...) {
        _debugException ("Thread destructor didn't terminate correctly");
        throw;
    }

    _manager->getAudioDriver()->getMainBuffer()->unBindAll (_ca->getCallId());

    if (_micData) {
        delete [] _micData;
        _micData = NULL;
    }

    if (_micDataConverted) {
        delete [] _micDataConverted;
        _micDataConverted = NULL;
    }

    if (_micDataEncoded) {
        delete [] _micDataEncoded;
        _micDataEncoded = NULL;
    }

    if (_spkrDataDecoded) {
        delete [] _spkrDataDecoded;
        _spkrDataDecoded = NULL;
    }

    if (_spkrDataConverted) {
        delete [] _spkrDataConverted;
        _spkrDataConverted = NULL;
    }

    delete _time;
    delete _converter;

    if (_audiocodec) {
        delete _audiocodec;
        _audiocodec = NULL;
    }

    if (_noiseState) {
        speex_preprocess_state_destroy (_noiseState);
    }

}

template <typename D>
void AudioRtpSession<D>::initBuffers()
{
    // Set sampling rate, main buffer choose the highest one
    _manager->getAudioDriver()->getMainBuffer()->setInternalSamplingRate (_codecSampleRate);

    // may be different than one already setted
    _converterSamplingRate = _manager->getAudioDriver()->getMainBuffer()->getInternalSamplingRate();

    // initialize SampleRate converter using AudioLayer's sampling rate
    // (internal buffers initialized with maximal sampling rate and frame size)
    _converter = new SamplerateConverter (_layerSampleRate, _layerFrameSize);

    int nbSamplesMax = (int) (_codecSampleRate * _layerFrameSize /1000) *2;
    _micData = new SFLDataFormat[nbSamplesMax];
    _micDataConverted = new SFLDataFormat[nbSamplesMax];
    _micDataEncoded = new unsigned char[nbSamplesMax*2];
    _spkrDataConverted = new SFLDataFormat[nbSamplesMax];
    _spkrDataDecoded = new SFLDataFormat[nbSamplesMax];


    memset (_micData, 0, nbSamplesMax*sizeof (SFLDataFormat));
    memset (_micDataConverted, 0, nbSamplesMax*sizeof (SFLDataFormat));
    memset (_micDataEncoded, 0, nbSamplesMax*2);
    memset (_spkrDataConverted, 0, nbSamplesMax*sizeof (SFLDataFormat));
    memset (_spkrDataDecoded, 0, nbSamplesMax*sizeof (SFLDataFormat));

    _manager->addStream (_ca->getCallId());
}

template <typename D>
void AudioRtpSession<D>::setSessionTimeouts (void)
{
    try {
        static_cast<D*> (this)->setSchedulingTimeout (schedulingTimeout);
        static_cast<D*> (this)->setExpireTimeout (expireTimeout);
    } catch (...) {
        _debugException ("Initialization failed while setting timeouts");
        throw AudioRtpSessionException();
    }
}

template <typename D>
void AudioRtpSession<D>::setSessionMedia (AudioCodec* audiocodec)
{
    _audiocodec = audiocodec;

    _debug ("RTP: Init codec payload %i", _audiocodec->getPayload());

    _codecSampleRate = _audiocodec->getClockRate();
    _codecFrameSize = _audiocodec->getFrameSize();

    // G722 requires timestamp to be incremented at 8 kHz
    if (_audiocodec->getPayload() == 9)
        _timestampIncrement = 160;
    else
        _timestampIncrement = _codecFrameSize;


    _debug ("RTP: Codec sampling rate: %d", _codecSampleRate);
    _debug ("RTP: Codec frame size: %d", _codecFrameSize);
    _debug ("RTP: RTP timestamp increment: %d", _timestampIncrement);

    // Even if specified as a 16 kHz codec, G722 requires rtp sending rate to be 8 kHz
    if (_audiocodec->getPayload() == 9) {
        _debug ("RTP: Setting G722 payload format");
        static_cast<D*> (this)->setPayloadFormat (ost::DynamicPayloadFormat ( (ost::PayloadType) _audiocodec->getPayload(), _audiocodec->getClockRate()));
    } else if (_audiocodec->hasDynamicPayload()) {
        _debug ("RTP: Setting dynamic payload format");
        static_cast<D*> (this)->setPayloadFormat (ost::DynamicPayloadFormat ( (ost::PayloadType) _audiocodec->getPayload(), _audiocodec->getClockRate()));
    } else if (!_audiocodec->hasDynamicPayload() && _audiocodec->getPayload() != 9) {
        _debug ("RTP: Setting static payload format");
        static_cast<D*> (this)->setPayloadFormat (ost::StaticPayloadFormat ( (ost::StaticPayloadType) _audiocodec->getPayload()));
    }

    if (_noiseState) {
        speex_preprocess_state_destroy (_noiseState);
        _noiseState = NULL;
    }

    _noiseState = speex_preprocess_state_init (_codecSampleRate, _codecFrameSize);
    int i=1;
    speex_preprocess_ctl (_noiseState, SPEEX_PREPROCESS_SET_DENOISE, &i);
    i=-20;
    speex_preprocess_ctl (_noiseState, SPEEX_PREPROCESS_SET_NOISE_SUPPRESS, &i);
    i=0;
    speex_preprocess_ctl (_noiseState, SPEEX_PREPROCESS_SET_AGC, &i);
    i=8000;
    speex_preprocess_ctl (_noiseState, SPEEX_PREPROCESS_SET_AGC_TARGET, &i);
    i=16000;
    speex_preprocess_ctl (_noiseState, SPEEX_PREPROCESS_SET_AGC_LEVEL, &i);
    i=0;
    speex_preprocess_ctl (_noiseState, SPEEX_PREPROCESS_SET_DEREVERB, &i);
    float f=0.0;
    speex_preprocess_ctl (_noiseState, SPEEX_PREPROCESS_SET_DEREVERB_DECAY, &f);
    f=0.0;
    speex_preprocess_ctl (_noiseState, SPEEX_PREPROCESS_SET_DEREVERB_LEVEL, &f);
}

template <typename D>
void AudioRtpSession<D>::setDestinationIpAddress (void)
{
    if (_ca == NULL) {
        _error ("RTP: Sipcall is gone.");
        throw AudioRtpSessionException();
    }

    _info ("RTP: Setting IP address for the RTP session");

    // Store remote ip in case we would need to forget current destination
    _remote_ip = ost::InetHostAddress (_ca->getLocalSDP()->get_remote_ip().c_str());

    if (!_remote_ip) {
        _warn ("RTP: Target IP address (%s) is not correct!",
               _ca->getLocalSDP()->get_remote_ip().data());
        return;
    }

    // Store remote port in case we would need to forget current destination
    _remote_port = (unsigned short) _ca->getLocalSDP()->get_remote_audio_port();

    _info ("RTP: New remote address for session: %s:%d",
           _ca->getLocalSDP()->get_remote_ip().data(), _remote_port);

    if (! static_cast<D*> (this)->addDestination (_remote_ip, _remote_port)) {
        _warn ("RTP: Can't add new destination to session!");
        return;
    }
}

template <typename D>
void AudioRtpSession<D>::updateDestinationIpAddress (void)
{
    // Destination address are stored in a list in ccrtp
    // This method remove the current destination entry

    if (!static_cast<D*> (this)->forgetDestination (_remote_ip, _remote_port, _remote_port+1))
        _warn ("RTP: Could not remove previous destination");

    // new destination is stored in call
    // we just need to recall this method
    setDestinationIpAddress();
}

template<typename D>
void AudioRtpSession<D>::putDtmfEvent (int digit)
{

    sfl::DtmfEvent *dtmf = new sfl::DtmfEvent();

    dtmf->payload.event = digit;
    dtmf->payload.ebit = false; 			// end of event bit
    dtmf->payload.rbit = false;  		// reserved bit
    dtmf->payload.duration = 1; 	        // duration for this event
    dtmf->newevent = true;
    dtmf->length = 1000;

    _eventQueue.push_back (dtmf);

    _debug ("RTP: Put Dtmf Event %d", _eventQueue.size());

}

template<typename D>
void AudioRtpSession<D>::sendDtmfEvent (sfl::DtmfEvent *dtmf)
{
    _debug ("RTP: Send Dtmf %d", _eventQueue.size());

    _timestamp += 160;

    // discard equivalent size of audio
    processDataEncode();

    // change Payload type for DTMF payload
    static_cast<D*> (this)->setPayloadFormat (ost::DynamicPayloadFormat ( (ost::PayloadType) 101, 8000));

    // Set marker in case this is a new Event
    if (dtmf->newevent)
        static_cast<D*> (this)->setMark (true);

    static_cast<D*> (this)->putData (_timestamp, (const unsigned char*) (& (dtmf->payload)), sizeof (ost::RTPPacket::RFC2833Payload));

    // This is no more a new event
    if (dtmf->newevent) {
        dtmf->newevent = false;
        static_cast<D*> (this)->setMark (false);
    }

    // get back the payload to audio
    static_cast<D*> (this)->setPayloadFormat (ost::StaticPayloadFormat ( (ost::StaticPayloadType) _audiocodec->getPayload()));

    // decrease length remaining to process for this event
    dtmf->length -= 160;

    dtmf->payload.duration += 1;

    // next packet is going to be the last one
    if ( (dtmf->length - 160) < 160)
        dtmf->payload.ebit = true;

    if (dtmf->length < 160) {
        delete dtmf;
        _eventQueue.pop_front();
    }
}

template <typename D>
bool onRTPPacketRecv (ost::IncomingRTPPkt&)
{
    _debug ("AudioRtpSession: onRTPPacketRecv");

    return true;
}


template <typename D>
int AudioRtpSession<D>::processDataEncode (void)
{
    assert (_audiocodec);
    assert (_audiolayer);

    int _mainBufferSampleRate = _manager->getAudioDriver()->getMainBuffer()->getInternalSamplingRate();

    // compute codec framesize in ms
    float fixed_codec_framesize = computeCodecFrameSize (_audiocodec->getFrameSize(), _audiocodec->getClockRate());

    // compute nb of byte to get coresponding to 20 ms at audio layer frame size (44.1 khz)
    int maxBytesToGet = computeNbByteAudioLayer (fixed_codec_framesize);

    // available bytes inside ringbuffer
    int availBytesFromMic = _manager->getAudioDriver()->getMainBuffer()->availForGet (_ca->getCallId());

    // set available byte to maxByteToGet
    int bytesAvail = (availBytesFromMic < maxBytesToGet) ? availBytesFromMic : maxBytesToGet;

    if (bytesAvail == 0) {
        memset (_micDataEncoded, 0, sizeof (SFLDataFormat));
        return _audiocodec->getFrameSize();
    }

    // Get bytes from micRingBuffer to data_from_mic
    int nbSample = _manager->getAudioDriver()->getMainBuffer()->getData (_micData , bytesAvail, 100, _ca->getCallId()) / sizeof (SFLDataFormat);

    if (!_micFadeInComplete)
        _micFadeInComplete = fadeIn (_micData, nbSample, &_micAmplFactor);

    if (nbSample == 0)
        return nbSample;

    // nb bytes to be sent over RTP
    int compSize = 0;

    // test if resampling is required
    if (_audiocodec->getClockRate() != _mainBufferSampleRate) {
        int nb_sample_up = nbSample;

        _nSamplesMic = nbSample;

        nbSample = _converter->downsampleData (_micData , _micDataConverted , _audiocodec->getClockRate(), _mainBufferSampleRate, nb_sample_up);

        compSize = _audiocodec->codecEncode (_micDataEncoded, _micDataConverted, nbSample*sizeof (SFLDataFormat));

    } else {

        _nSamplesMic = nbSample;

        // no resampling required
        compSize = _audiocodec->codecEncode (_micDataEncoded, _micData, nbSample*sizeof (SFLDataFormat));

    }

    return compSize;
}

template <typename D>
void AudioRtpSession<D>::processDataDecode (unsigned char * spkrData, unsigned int size)
{
    if (_audiocodec != NULL) {


        int _mainBufferSampleRate = _manager->getAudioDriver()->getMainBuffer()->getInternalSamplingRate();

        // Return the size of data in bytes
        int expandedSize = _audiocodec->codecDecode (_spkrDataDecoded , spkrData , size);

        // buffer _receiveDataDecoded ----> short int or int16, coded on 2 bytes
        int nbSample = expandedSize / sizeof (SFLDataFormat);

        if (!_spkrFadeInComplete)
            _spkrFadeInComplete = fadeIn (_spkrDataDecoded, nbSample, &_spkrAmplFactor);

        // test if resampling is required
        if (_audiocodec->getClockRate() != _mainBufferSampleRate) {

            // Do sample rate conversion
            int nb_sample_down = nbSample;

            nbSample = _converter->upsampleData (_spkrDataDecoded, _spkrDataConverted, _codecSampleRate, _mainBufferSampleRate, nb_sample_down);

            // Store the number of samples for recording
            _nSamplesSpkr = nbSample;

            speex_preprocess_run (_noiseState, _spkrDataConverted);

            // put data in audio layer, size in byte
            _manager->getAudioDriver()->getMainBuffer()->putData (_spkrDataConverted, nbSample * sizeof (SFLDataFormat), 100, _ca->getCallId());


        } else {
            // Store the number of samples for recording
            _nSamplesSpkr = nbSample;

            // speex_preprocess_run(_noiseState, _spkrDataDecoded);

            // put data in audio layer, size in byte
            _manager->getAudioDriver()->getMainBuffer()->putData (_spkrDataDecoded, expandedSize, 100, _ca->getCallId());
        }

        // Notify (with a beep) an incoming call when there is already a call
        if (_manager->incomingCallWaiting() > 0) {
            _countNotificationTime += _time->getSecond();
            int countTimeModulo = _countNotificationTime % 5000;

            // _debug("countNotificationTime: %d\n", countNotificationTime);
            // _debug("countTimeModulo: %d\n", countTimeModulo);
            if ( (countTimeModulo - _countNotificationTime) < 0) {
                _manager->notificationIncomingCall();
            }

            _countNotificationTime = countTimeModulo;
        }

    }
}

template <typename D>
void AudioRtpSession<D>::sendMicData()
{
    // STEP:
    //   1. get data from mic
    //   2. convert it to int16 - good sample, good rate
    //   3. encode it
    //   4. send it

    // Increment timestamp for outgoing packet
    _timestamp += _timestampIncrement;

    if (!_audiolayer) {
        _debug ("No audiolayer available for MIC");
        return;
    }

    if (!_audiocodec) {
        _debug ("No audiocodec available for MIC");
        return;
    }

    int compSize = processDataEncode();

    // putData put the data on RTP queue, sendImmediate bypass this queue
    static_cast<D*> (this)->putData (_timestamp, _micDataEncoded, compSize);
}


template <typename D>
void AudioRtpSession<D>::receiveSpeakerData ()
{

    if (!_audiolayer) {
        _debug ("No audiolayer available for speaker");
        return;
    }

    if (!_audiocodec) {
        _debug ("No audiocodec available for speaker");
        return;
    }

    const ost::AppDataUnit* adu = NULL;

    int packetTimestamp = static_cast<D*> (this)->getFirstTimestamp();

    adu = static_cast<D*> (this)->getData (packetTimestamp);

    if (!adu) {
        return;
    }

    unsigned char* spkrDataIn = NULL;
    unsigned int size = 0;

    if (adu) {

        spkrDataIn  = (unsigned char*) adu->getData(); // data in char
        size = adu->getSize(); // size in char

    } else {
        _debug ("No RTP packet available !!!!!!!!!!!!!!!!!!!!!!!\n");
    }

    // DTMF over RTP, size must be over 4 in order to process it as voice data
    if (size > 4) {
        processDataDecode (spkrDataIn, size);
    }

    delete adu;
}

template <typename D>
bool AudioRtpSession<D>::fadeIn (SFLDataFormat *audio, int size, SFLDataFormat *factor)
{

    // apply amplitude factor;
    while (size) {
        size--;
        audio[size] /= *factor;
    }

    // decrease factor
    *factor /= FADEIN_STEP_SIZE;

    // if factor reach 0, thsi function should no be called anymore
    if (*factor == 0)
        return true;

    return false;
}

template <typename D>
int AudioRtpSession<D>::startRtpThread (AudioCodec* audiocodec)
{
    _debug ("RTP: Starting main thread");
    setSessionTimeouts();
    setSessionMedia (audiocodec);
    initBuffers();
    return start (_mainloopSemaphore);
}

template <typename D>
void AudioRtpSession<D>::run ()
{

    // Timestamp must be initialized randomly
    _timestamp = static_cast<D*> (this)->getCurrentTimestamp();

    int threadSleep = 0;

    if (_codecSampleRate != 0) {
        threadSleep = (_codecFrameSize * 1000) / _codecSampleRate;
    } else {
        threadSleep = _layerFrameSize;
    }

    TimerPort::setTimer (threadSleep);

    if (_audiolayer == NULL) {
        _error ("RTP: Error: Audiolayer is null, cannot start the audio stream");
        throw AudioRtpSessionException();
    }

    _ca->setRecordingSmplRate (_audiocodec->getClockRate());

    // Start audio stream (if not started) AND flush all buffers (main and urgent)
    _manager->getAudioDriver()->startStream();
    static_cast<D*> (this)->startRunning();


    _debug ("RTP: Entering mainloop for call %s",_ca->getCallId().c_str());

    _manager->getAudioDriver()->getMainBuffer()->getInternalSamplingRate();

    while (!testCancel()) {

        // Reset timestamp to make sure the timing information are up to date
        if (_timestampCount > RTP_TIMESTAMP_RESET_FREQ) {
            _timestamp = static_cast<D*> (this)->getCurrentTimestamp();
            _timestampCount = 0;
        }

        _timestampCount++;


        _manager->getAudioLayerMutex()->enter();

        // converterSamplingRate = _audiolayer->getMainBuffer()->getInternalSamplingRate();
        _converterSamplingRate = _manager->getAudioDriver()->getMainBuffer()->getInternalSamplingRate();

        // Send session
        if (_eventQueue.size() > 0) {
            sendDtmfEvent (_eventQueue.front());
        } else {
            sendMicData ();
        }

        // Recv session
        receiveSpeakerData ();

        _manager->getAudioLayerMutex()->leave();

        // Let's wait for the next transmit cycle
        Thread::sleep (TimerPort::getTimer());
        TimerPort::incTimer (threadSleep);
    }

    _debug ("RTP: Left main loop for call%s", _ca->getCallId().c_str());
}

}
#endif // __AUDIO_RTP_SESSION_H__

