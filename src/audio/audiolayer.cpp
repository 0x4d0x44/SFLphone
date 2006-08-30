/*
 *  Copyright (C) 2005 Savoir-Faire Linux inc.
 *  Author: Yan Morin <yan.morin@savoirfairelinux.com>
 *  Author: Jerome Oufella <jerome.oufella@savoirfairelinux.com> 
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
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

#include <stdio.h>
#include <stdlib.h>

#include "audiolayer.h"
#include "../global.h"
#include "../manager.h"

//#define SFL_TEST
//#define SFL_TEST_SINE

#ifdef SFL_TEST_SINE
#include <cmath>
#endif

AudioLayer::AudioLayer(ManagerImpl* manager)
  : _urgentRingBuffer(SIZEBUF)
  , _mainSndRingBuffer(SIZEBUF)
  , _micRingBuffer(SIZEBUF)
  , _stream(NULL)
  , _errorMessage("")
  , _manager(manager)
{
  _sampleRate = 8000;
  
  _inChannel  = 1; // don't put in stereo
  _outChannel = 1; // don't put in stereo

  try {
     portaudio::AutoSystem autoSys;
     portaudio::System::initialize();
  }
  catch (const portaudio::PaException &e) {
    setErrorMessage(e.paErrorText());
  }
  catch (const portaudio::PaCppException &e) {
    setErrorMessage(e.what());
  } // std::runtime_error &e     (e.what())
  catch (...) {
    setErrorMessage("Unknown type error in portaudio initialization");
  }

#ifdef SFL_TEST_SINE
  leftPhase_ = 0;
  tableSize_ = 200;
  const double PI = 3.14159265;
  table_ = new float[tableSize_];
  for (int i = 0; i < tableSize_; ++i)
  {
    table_[i] = 0.125f * (float)sin(((double)i/(double)tableSize_)*PI*2.);
    _debug("%9.8f\n", table_[i]);
  }
#endif
}

// Destructor
AudioLayer::~AudioLayer (void) 
{ 
  stopStream();
  closeStream();

  try {
    portaudio::System::terminate();
  } catch (const portaudio::PaException &e) {
    _debug("! AL: Catch an exception when portaudio tried to terminate\n");
  }
#ifdef SFL_TEST_SINE
  delete [] table_;
#endif
}

void
AudioLayer::closeStream (void) 
{
  ost::MutexLock guard(_mutex);
  if(_stream) {
    _stream->close();
    delete _stream; _stream = NULL;
  }
}

bool
AudioLayer::hasStream(void) {
  ost::MutexLock guard(_mutex);
  return (_stream!=0 ? true : false); 
}


void
AudioLayer::openDevice (int indexIn, int indexOut, int sampleRate) 
{
  closeStream();

  _sampleRate = sampleRate;

  int portaudioFramePerBuffer = FRAME_PER_BUFFER; //=FRAME_PER_BUFFER; //= paFramesPerBufferUnspecified;

  int nbDevice = getDeviceCount();
  if (nbDevice == 0) {
    _debug("Portaudio detect no sound card.");
    return;
  } else {
    if (indexIn >= nbDevice) {
      _debug(" Portaudio auto-select device #0 for input because device #%02d is not found\n", indexIn);
      indexIn = 0;
    }
    if (indexOut >= nbDevice) {
      _debug(" Portaudio auto-select device #0 for output because device #%02d is not found\n", indexOut);
      indexOut = 0;
    }

    _debug(" Setting audiolayer: device     in=%2d, out=%2d\n", indexIn, indexOut);
    _debug("                   : nb channel in=%2d, out=%2d\n", _inChannel, _outChannel);
    _debug("                   : sample rate=%5d, format=%s\n", _sampleRate, SFLPortaudioFormatString);
    _debug("                   : frame per buffer=%d\n", portaudioFramePerBuffer);
  }

  try {
    // Set up the parameters required to open a (Callback)Stream:
    portaudio::DirectionSpecificStreamParameters 
      inParams(portaudio::System::instance().deviceByIndex(indexIn), 
	     _inChannel, SFLPortaudioFormat, true, 
	     portaudio::System::instance().deviceByIndex(indexIn).defaultLowInputLatency(), 
	     NULL);

     portaudio::DirectionSpecificStreamParameters outParams(
                portaudio::System::instance().deviceByIndex(indexOut), 
	        _outChannel, SFLPortaudioFormat, true, 
	        portaudio::System::instance().deviceByIndex(indexOut).defaultLowOutputLatency(), 
	        NULL);

    // like audacity
    // DON'T USE paFramesPerBufferUnspecified, it's 32, instead of 160 for FRAME_PER_BUFFER
    // DON'T USE paDitherOff or paClipOff, 
    // FRAME_PER_BUFFER | paFramesPerBufferUnspecified
    // paNoFlag | paClipOff || paDitherOff | paPrimeOutputBuffersUsingStreamCallback | paNeverDropInput
     
    portaudio::StreamParameters const params(
		inParams,
		outParams, 
		_sampleRate, portaudioFramePerBuffer, paClipOff);
		
    // Create (and open) a new Stream, using the AudioLayer::audioCallback
    ost::MutexLock guard(_mutex);
#ifdef SFL_TEST
    _stream = new portaudio::MemFunCallbackStream<AudioLayer>(params, *this, &AudioLayer::miniAudioCallback);
#else
    _stream = new portaudio::MemFunCallbackStream<AudioLayer>(params,*this, &AudioLayer::audioCallback);
#endif
 
  }
  catch (const portaudio::PaException &e) {
    setErrorMessage(e.paErrorText());
    _debug("Portaudio openDevice error: %s\n", e.paErrorText());
  }
  catch (const portaudio::PaCppException &e) {
    setErrorMessage(e.what());
    _debug("Portaudio openDevice error: %s\n", e.what());
  } // std::runtime_error &e     (e.what())
  catch (...) {
    setErrorMessage("Unknown type error in portaudio openDevice");
    _debug("Portaudio openDevice: unknown error\n");
  }

}

int
AudioLayer::getDeviceCount()
{
  return portaudio::System::instance().deviceCount();
}

AudioDevice*
AudioLayer::getAudioDeviceInfo(int index, int ioDeviceMask)
{

  try {
    portaudio::System& sys = portaudio::System::instance();
    portaudio::Device& device = sys.deviceByIndex(index);
    int deviceIsSupported = false;

    if (ioDeviceMask == InputDevice && !device.isOutputOnlyDevice()) {
      deviceIsSupported = true;
    } else if (ioDeviceMask == OutputDevice && !device.isInputOnlyDevice()) {
      deviceIsSupported = true;
    } else if (device.isFullDuplexDevice()) {
      deviceIsSupported = true;
    }

    if (deviceIsSupported) {
      AudioDevice* audiodevice = new AudioDevice(index, device.hostApi().name(), device.name());
      if (audiodevice) {
        audiodevice->setRate(device.defaultSampleRate());
      }
      return audiodevice;
    }
  } catch (...) {
    return 0;
  }
  return 0;
}



void
AudioLayer::startStream(void) 
{
  try {
    ost::MutexLock guard(_mutex);
    if (_stream && !_stream->isActive()) {
        _debug("- AL Action: Starting sound stream\n");
        _stream->start();
    } else { 
      _debug ("* AL Info: Stream doesn't exist or is already active\n");
    }
  } catch (const portaudio::PaException &e) {
    _debugException("! AL: Portaudio error: error on starting audiolayer stream");
    throw;
  } catch(...) {
    _debugException("! AL: Stream start error");
    throw;
  }
}
	
void
AudioLayer::stopStream(void) 
{
  _debug("- AL Action: Stopping sound stream\n");
  try {
    ost::MutexLock guard(_mutex);
    if (_stream && !_stream->isStopped()) {
      _stream->stop();
      _mainSndRingBuffer.flush();
      _urgentRingBuffer.flush();
      _micRingBuffer.flush();
    }
  } catch (const portaudio::PaException &e) {
    _debugException("! AL: Portaudio error: stoping audiolayer stream failed");
    throw;
  } catch(...) {
    _debugException("! AL: Stream stop error");
    throw;
  }
}

void
AudioLayer::sleep(int msec) 
{
  if (_stream) {
    portaudio::System::instance().sleep(msec);
  }
}

bool
AudioLayer::isStreamActive (void) 
{
  ost::MutexLock guard(_mutex);
  try {
    if(_stream && _stream->isActive()) {
      return true;
    }
  } catch (const portaudio::PaException &e) {
      _debugException("! AL: Portaudio error: isActive returned an error");
  }
  return false;
}

int 
AudioLayer::putMain(void* buffer, int toCopy)
{
  ost::MutexLock guard(_mutex);
  if (_stream) {
    int a = _mainSndRingBuffer.AvailForPut();
    if ( a >= toCopy ) {
      return _mainSndRingBuffer.Put(buffer, toCopy);
    } else {
      return _mainSndRingBuffer.Put(buffer, a);
    }
  }
  return 0;
}

void
AudioLayer::flushMain()
{
  ost::MutexLock guard(_mutex);
  _mainSndRingBuffer.flush();
}

int
AudioLayer::putUrgent(void* buffer, int toCopy)
{
  ost::MutexLock guard(_mutex);
  if (_stream) {
    int a = _urgentRingBuffer.AvailForPut();
    if ( a >= toCopy ) {
      return _urgentRingBuffer.Put(buffer, toCopy);
    } else {
      return _urgentRingBuffer.Put(buffer, a);
    }
  }
  return 0;
}

int
AudioLayer::canGetMic()
{
  if (_stream) {
    return _micRingBuffer.AvailForGet();
  } else {
    return 0;
  }
}

int 
AudioLayer::getMic(void *buffer, int toCopy)
{
  if(_stream) {
    return _micRingBuffer.Get(buffer, toCopy, 100);
  } else {
    return 0;
  }
}

void
AudioLayer::flushMic()
{
  _micRingBuffer.flush();
}

bool
AudioLayer::isStreamStopped (void) 
{
  ost::MutexLock guard(_mutex);
  try {
    if(_stream && _stream->isStopped()) {
      return true;
    }
  } catch (const portaudio::PaException &e) {
      _debugException("! AL: Portaudio error: isStopped returned an exception");
  }
  return false;
}

int 
AudioLayer::audioCallback (const void *inputBuffer, void *outputBuffer, 
			   unsigned long framesPerBuffer, 
			   const PaStreamCallbackTimeInfo *timeInfo, 
			   PaStreamCallbackFlags statusFlags) {

  (void) timeInfo;
  (void) statusFlags;
	
  SFLDataFormat *in  = (SFLDataFormat *) inputBuffer;
  SFLDataFormat *out = (SFLDataFormat *) outputBuffer;
  int toGet; 
  int toPut;
  int urgentAvail; // number of data right and data left
  int normalAvail; // number of data right and data left
  int micAvailPut;
  unsigned short spkrVolume = _manager->getSpkrVolume();
  unsigned short micVolume  = _manager->getMicVolume();

  // AvailForGet tell the number of chars inside the buffer
  // framePerBuffer are the number of data for one channel (left)
  urgentAvail = _urgentRingBuffer.AvailForGet();
  if (urgentAvail > 0) {
    // Urgent data (dtmf, incoming call signal) come first.		
    toGet = (urgentAvail < (int)(framesPerBuffer * sizeof(SFLDataFormat))) ? urgentAvail : framesPerBuffer * sizeof(SFLDataFormat);
    _urgentRingBuffer.Get(out, toGet, spkrVolume);
    // Consume the regular one as well (same amount of bytes)
    _mainSndRingBuffer.Discard(toGet);
  } else {
    AudioLoop* tone = _manager->getTelephoneTone();
    if ( tone != 0) {
      tone->getNext(out, framesPerBuffer, spkrVolume);
    } else if ( (tone=_manager->getTelephoneFile()) != 0 ) {
      tone->getNext(out, framesPerBuffer, spkrVolume);
    } else {
      // If nothing urgent, play the regular sound samples
      normalAvail = _mainSndRingBuffer.AvailForGet();
      toGet = (normalAvail < (int)(framesPerBuffer * sizeof(SFLDataFormat))) ? normalAvail : framesPerBuffer * sizeof(SFLDataFormat);

      if (toGet) {
        _mainSndRingBuffer.Get(out, toGet, spkrVolume);
      } else {
        bzero(out, framesPerBuffer * sizeof(SFLDataFormat));
      }
    }
  }

  // Additionally handle the mic's audio stream 
  micAvailPut = _micRingBuffer.AvailForPut();
  toPut = (micAvailPut <= (int)(framesPerBuffer * sizeof(SFLDataFormat))) ? micAvailPut : framesPerBuffer * sizeof(SFLDataFormat);
  //_debug("AL: Nb sample: %d char, [0]=%f [1]=%f [2]=%f\n", toPut, in[0], in[1], in[2]);
  _micRingBuffer.Put(in, toPut, micVolume);

  return paContinue;
}

int 
AudioLayer::miniAudioCallback (const void *inputBuffer, void *outputBuffer, 
			   unsigned long framesPerBuffer, 
			   const PaStreamCallbackTimeInfo *timeInfo, 
			   PaStreamCallbackFlags statusFlags) {
  (void) timeInfo;
  (void) statusFlags;
	
 _debug("mini audio callback!!\n");
#ifdef SFL_TEST_SINE
  assert(outputBuffer != NULL);

  float *out = static_cast<float *>(outputBuffer);
  for (unsigned int i = 0; i < framesPerBuffer; ++i) {
    out[i] = table_[leftPhase_];
    leftPhase_ += 1;
    if (leftPhase_ >= tableSize_) {
      leftPhase_ -= tableSize_;
    }
  }
#else
  SFLDataFormat *out = (SFLDataFormat *) outputBuffer;
  AudioLoop* tone = _manager->getTelephoneTone();
  if ( tone != 0) {
    //_debug("Frames Per Buffer: %d\n", framesPerBuffer);
    tone->getNext(out, framesPerBuffer, 100);
  }
#endif

  return paContinue;
}
