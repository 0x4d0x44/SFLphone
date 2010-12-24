/*
 *  Copyright (C) 2004, 2005, 2006, 2009, 2008, 2009, 2010 Savoir-Faire Linux Inc.
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

#include "AudioRtpRecordHandler.h"

#include "audio/audiolayer.h"
#include "manager.h"

namespace sfl
{

static const SFLDataFormat initFadeinFactor = 32000;

AudioRtpRecord::AudioRtpRecord () : _audioCodec (NULL)
    , _hasDynamicPayloadType (false)
    , _micData (NULL)
    , _micDataConverted (NULL)
    , _micDataEncoded (NULL)
    , _spkrDataDecoded (NULL)
    , _spkrDataConverted (NULL)
    , _converter (NULL)
    , _codecSampleRate (0)
    , _codecFrameSize (0)
    , _micFadeInComplete (false)
    , _spkrFadeInComplete (false)
    , _micAmplFactor (initFadeinFactor)
    , _spkrAmplFactor (initFadeinFactor)
    , _audioProcess (NULL)
    , _noiseSuppress (NULL)
{

}


AudioRtpRecord::~AudioRtpRecord()
{
    if (_micData)
        delete [] _micData;

    _micData = NULL;

    if (_micDataConverted)
        delete [] _micDataConverted;

    _micDataConverted = NULL;

    if (_micDataEncoded)
        delete [] _micDataEncoded;

    _micDataEncoded = NULL;

    if (_spkrDataDecoded)
        delete [] _spkrDataDecoded;

    _spkrDataDecoded = NULL;

    if (_spkrDataConverted)
        delete [] _spkrDataConverted;

    _spkrDataConverted = NULL;


    if (_converter)
        delete _converter;

    _converter = NULL;

    if (_audioCodec) {
        delete _audioCodec;
        _audioCodec = NULL;
    }

    if (_audioProcess) {
        delete _audioProcess;
        _audioProcess = NULL;
    }

    if (_noiseSuppress) {
        delete _noiseSuppress;
        _noiseSuppress = NULL;
    }
}


AudioRtpRecordHandler::AudioRtpRecordHandler (ManagerImpl *manager, SIPCall *ca) : _audioRtpRecord (), _ca (ca) {}


AudioRtpRecordHandler::~AudioRtpRecordHandler() {}

void AudioRtpRecordHandler::setRtpMedia (AudioCodec* audioCodec)
{

    // Set varios codec info to reduce indirection
    _audioRtpRecord._audioCodec = audioCodec;
    _audioRtpRecord._codecPayloadType = audioCodec->getPayload();
    _audioRtpRecord._codecSampleRate = audioCodec->getClockRate();
    _audioRtpRecord._codecFrameSize = audioCodec->getFrameSize();
    _audioRtpRecord._hasDynamicPayloadType = audioCodec->hasDynamicPayload();
}


void AudioRtpRecordHandler::updateRtpMedia (AudioCodec *audioCodec)
{
    if (_audioRtpRecord._audioCodec) {
        delete _audioRtpRecord._audioCodec;
        _audioRtpRecord._audioCodec = NULL;
    }

    _audioRtpRecord._audioCodec = audioCodec;
    _audioRtpRecord._codecPayloadType = audioCodec->getPayload();
    _audioRtpRecord._codecSampleRate = audioCodec->getClockRate();
    _audioRtpRecord._codecFrameSize = audioCodec->getFrameSize();
    _audioRtpRecord._hasDynamicPayloadType = audioCodec->hasDynamicPayload();
}

void AudioRtpRecordHandler::init()
{
}

void AudioRtpRecordHandler::initBuffers()
{
    int codecSampleRate = _audioRtpRecord._codecSampleRate;

    // Set sampling rate, main buffer choose the highest one
    Manager::instance().getMainBuffer()->setInternalSamplingRate (codecSampleRate);

    // initialize SampleRate converter using AudioLayer's sampling rate
    // (internal buffers initialized with maximal sampling rate and frame size)
    _audioRtpRecord._converter = new SamplerateConverter ();

    int nbSamplesMax = (int) ( (getCodecSampleRate() * getCodecFrameSize() / 1000));
    _audioRtpRecord._micData = new SFLDataFormat[nbSamplesMax];
    _audioRtpRecord._micDataConverted = new SFLDataFormat[nbSamplesMax];
    _audioRtpRecord._micDataEncoded = new unsigned char[nbSamplesMax * 2];
    _audioRtpRecord._spkrDataConverted = new SFLDataFormat[nbSamplesMax];
    _audioRtpRecord._spkrDataDecoded = new SFLDataFormat[nbSamplesMax];

    Manager::instance().addStream (_ca->getCallId());
}

void AudioRtpRecordHandler::initNoiseSuppress()
{
    NoiseSuppress *noiseSuppress = new NoiseSuppress (getCodecFrameSize(), getCodecSampleRate());
    AudioProcessing *processing = new AudioProcessing (noiseSuppress);

    _audioRtpRecord._noiseSuppress = noiseSuppress;
    _audioRtpRecord._audioProcess = processing;
}

void AudioRtpRecordHandler::putDtmfEvent (int digit)
{
    sfl::DtmfEvent *dtmf = new sfl::DtmfEvent();
    dtmf->payload.event = digit;
    dtmf->payload.ebit = false; // end of event bit
    dtmf->payload.rbit = false; // reserved bit
    dtmf->payload.duration = 1; // duration for this event
    dtmf->newevent = true;
    dtmf->length = 1000;
    getEventQueue()->push_back (dtmf);
    _debug ("AudioRtpSession: Put Dtmf Event %d", getEventQueue()->size());
}

int AudioRtpRecordHandler::processDataEncode (void)
{
    AudioCodec *audioCodec = _audioRtpRecord._audioCodec;

    SFLDataFormat *micData = _audioRtpRecord._micData;
    unsigned char *micDataEncoded = _audioRtpRecord._micDataEncoded;
    SFLDataFormat *micDataConverted = _audioRtpRecord._micDataConverted;

    assert (audioCodec);

    int codecFrameSize = audioCodec->getFrameSize();
    int codecSampleRate = audioCodec->getClockRate();

    int mainBufferSampleRate = Manager::instance().getMainBuffer()->getInternalSamplingRate();

    // compute codec framesize in ms
    float fixedCodecFramesize = computeCodecFrameSize (codecFrameSize, codecSampleRate);

    // compute nb of byte to get coresponding to 20 ms at audio layer frame size (44.1 khz)
    int bytesToGet = computeNbByteAudioLayer (fixedCodecFramesize);

    // available bytes inside ringbuffer
    int availBytesFromMic = Manager::instance().getMainBuffer()->availForGet (_ca->getCallId());

    if (availBytesFromMic < bytesToGet)
        return 0;

    // Get bytes from micRingBuffer to data_from_mic
    int nbSample = Manager::instance().getMainBuffer()->getData (micData, bytesToGet, 100, _ca->getCallId()) / sizeof (SFLDataFormat);

    // process mic fade in
    if (!_audioRtpRecord._micFadeInComplete)
        _audioRtpRecord._micFadeInComplete = fadeIn (micData, nbSample, &_audioRtpRecord._micAmplFactor);

    if (nbSample == 0)
        return nbSample;

    // nb bytes to be sent over RTP
    int compSize = 0;

    // test if resampling is required
    if (codecSampleRate != mainBufferSampleRate) {

        int nbSampleUp = nbSample;

        nbSample = _audioRtpRecord._converter->downsampleData (micData, micDataConverted, codecSampleRate, mainBufferSampleRate, nbSampleUp);

        if (Manager::instance().audioPreference.getNoiseReduce())
            _audioRtpRecord._audioProcess->processAudio (micDataConverted, nbSample * sizeof (SFLDataFormat));

        compSize = audioCodec->codecEncode (micDataEncoded, micDataConverted, nbSample * sizeof (SFLDataFormat));
    } else {
        if (Manager::instance().audioPreference.getNoiseReduce())
            _audioRtpRecord._audioProcess->processAudio (micData, nbSample * sizeof (SFLDataFormat));

        // no resampling required
        compSize = audioCodec->codecEncode (micDataEncoded, micData, nbSample * sizeof (SFLDataFormat));
    }

    return compSize;
}

void AudioRtpRecordHandler::processDataDecode (unsigned char *spkrData, unsigned int size)
{
    AudioCodec *audioCodec = _audioRtpRecord._audioCodec;
    // AudioLayer *audioLayer = Manager::instance().getAudioDriver();

    if (!audioCodec)
        return;

    int codeFrameSize = audioCodec->getFrameSize();
    int codecSampleRate = audioCodec->getClockRate();

    SFLDataFormat *spkrDataDecoded = _audioRtpRecord._spkrDataConverted;
    SFLDataFormat *spkrDataConverted = _audioRtpRecord._spkrDataDecoded;

    int mainBufferSampleRate = Manager::instance().getMainBuffer()->getInternalSamplingRate();

    // Return the size of data in bytes
    int expandedSize = audioCodec->codecDecode (spkrDataDecoded , spkrData , size);

    // buffer _receiveDataDecoded ----> short int or int16, coded on 2 bytes
    int nbSample = expandedSize / sizeof (SFLDataFormat);

    if (!_audioRtpRecord._spkrFadeInComplete)
        _audioRtpRecord._spkrFadeInComplete = fadeIn (spkrDataDecoded, nbSample, &_audioRtpRecord._micAmplFactor);

    // test if resampling is required
    if (codecSampleRate != mainBufferSampleRate) {

        // Do sample rate conversion
        int nbSampleDown = nbSample;

        nbSample = _audioRtpRecord._converter->upsampleData (spkrDataDecoded, spkrDataConverted, codecSampleRate, mainBufferSampleRate, nbSampleDown);

        // put data in audio layer, size in byte
        Manager::instance().getMainBuffer()->putData (spkrDataConverted, nbSample * sizeof (SFLDataFormat), 100, _ca->getCallId());


    } else {
        // put data in audio layer, size in byte
        Manager::instance().getMainBuffer()->putData (spkrDataDecoded, expandedSize, 100, _ca->getCallId());
    }
}

bool AudioRtpRecordHandler::fadeIn (SFLDataFormat *audio, int size, SFLDataFormat *factor)
{

    // if factor reach 0, this function should no be called anymore
    if (*factor <= 0)
        return true;

    // apply amplitude factor;
    while (size) {
        size--;
        audio[size] /= *factor;
    }

    // decrease factor
    *factor /= FADEIN_STEP_SIZE;

    return false;

}

}

