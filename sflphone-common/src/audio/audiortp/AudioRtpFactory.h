/*
 *  Copyright (C) 2004-2009 Savoir-Faire Linux inc.
 *  Author: Pierre-Luc Bacon <pierre-luc.bacon@savoirfairelinux.com>
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
 */

#ifndef __AUDIO_RTP_FACTORY_H__
#define __AUDIO_RTP_FACTORY_H__

#include <exception>
#include <cc++/thread.h>

#include "sip/sipcall.h"

class SIPCall;

namespace sfl {

    // Possible kind of rtp session
    typedef enum RtpMethod {
        Symmetric,
        Zrtp,
        Sdes
    } RtpMethod;

    class UnsupportedRtpSessionType: public std::exception
    {
      virtual const char* what() const throw()
      {
        return "Could not create RTP session of the given type";
      }
    };

    class AudioRtpFactoryException: public std::exception
    {
      virtual const char* what() const throw()
      {
        return "An AudioRtpFactoryException occured";
      }
    };

    class AudioRtpFactory {
        public:
        AudioRtpFactory();
        AudioRtpFactory(SIPCall * ca);
        ~AudioRtpFactory();

        /**
         * Lazy instantiation method. Create a new RTP session of a given 
         * type according to the content of the configuration file. 
         * @param ca A pointer on a SIP call
         * @return A new AudioRtpSession object
         */
        void initAudioRtpSession(SIPCall *ca);
        
        /**
         * Start the audio rtp thread of the type specified in the configuration
         * file. initAudioRtpSession must have been called prior to that.
         * @param None
         */
        void start();
     
         /**
         * Stop the audio rtp thread of the type specified in the configuration
         * file. initAudioRtpSession must have been called prior to that.
         * @param None
         */
        void stop();
          
          /** 
           * @param None
           * @return The internal audio rtp thread of the type specified in the configuration
           * file. initAudioRtpSession must have been called prior to that. 
           */  
        inline void * getAudioRtpSession(void) { return _rtpSession; }
        
        private:
           void * _rtpSession;
           RtpMethod _rtpSessionType;
           ost::Mutex _audioRtpThreadMutex;
    };
}
#endif // __AUDIO_RTP_FACTORY_H__