/*
 *  Copyright (C) 2004, 2005, 2006, 2009, 2008, 2009, 2010 Savoir-Faire Linux Inc.
 *  Author : Alexandre Savard <alexandre.savard@savoirfairelinux.com>
 *
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

#ifndef __MAIN_BUFFER__
#define __MAIN_BUFFER__

#include <map>
#include <set>
#include <cc++/thread.h> // for ost::Mutex
#include <string>

#include "global.h"
#include "call.h"
#include "ringbuffer.h"
#include "AudioFrame.h"

typedef std::map<CallID, RingBuffer*> RingBufferMap;

typedef std::set<CallID> CallIDSet;

typedef std::map<CallID, CallIDSet*> CallIDMap;

class MainBuffer
{

    public:

        MainBuffer();

        ~MainBuffer();

        void setInternalSamplingRate (int sr);

        int getInternalSamplingRate() {
            return _internalSamplingRate;
        }

        /**
         * Bind together two audio streams so taht a client will be able
         * to put and get data specifying its callid only.
         */
        void bindCallID (CallID call_id1, CallID call_id2 = default_id);

        /**
         * Add a new call_id to unidirectional outgoing stream
         * \param call_id New call id to be added for this stream
         * \param process_id Process that require this stream
         */
        void bindHalfDuplexOut (CallID process_id, CallID call_id = default_id);

        /**
         * Unbind two calls
         */
        void unBindCallID (CallID call_id1, CallID call_id2 = default_id);

        /**
         * Unbind a unidirectional stream
         */
        void unBindHalfDuplexOut (CallID process_id, CallID call_id = default_id);

        /**
         * Unbind coresponding ringbuffer from audio layer
         */
        void unBindAll (CallID call_id);

        /**
         * Unbind coresponding ringbuffer from audio recorder
         */
        void unBindAllHalfDuplexOut (CallID process_id);

        /**
         * Put data into the coresponding audio ringbuffer
         */
        int putData (void *buffer, int toCopy, unsigned short volume = 100, CallID call_id = default_id);

        /**
         * Get data from the coresponding audio ringbuffer(s). Data may come from several buffers in case of a conference
         */
        int getData (void *buffer, int toCopy, unsigned short volume = 100, CallID call_id = default_id);

        /**
         * Return the available number of bytes to store data in ringbuffer
         */
        int availForPut (CallID call_id = default_id);

        /**
         * Return the available number of byte already stored in ringbuffer(s)
         */
        int availForGet (CallID call_id = default_id);

        /**
         * Discard the specified number of data in ringbuffer(s). Usefull when dropping frames for synchronisation
         */
        int discard (int toDiscard, CallID call_id = default_id);

        /**
         * Reset internal ringbuffers coresponding to this callID
         */
        void flush (CallID call_id = default_id);

        /**
         * Reset the all buffers
         */
        void flushAllBuffers();

        void flushDefault();

        void syncBuffers (CallID call_id);

        void stateInfo();

    private:

        /**
         * Get teh call ids to which this call is bound
         */
        CallIDSet* getCallIDSet (CallID call_id);

        /**
         * Create a new call id set for this call
         */
        bool createCallIDSet (CallID set_id);

        /**
         * Remove this call id set
         */
        bool removeCallIDSet (CallID set_id);

        /**
         * Add a new call id to this set
         */
        void addCallIDtoSet (CallID set_id, CallID call_id);

        /**
         * Remove a call id from a set
         */
        void removeCallIDfromSet (CallID set_id, CallID call_id);

        /**
         * Create a new ringbuffer with default readpointer
         */
        RingBuffer* createRingBuffer (CallID call_id);

        /**
         * Remove corresponding ring buffer
         */
        bool removeRingBuffer (CallID call_id);

        /**
         * Get corresponding ring buffer
         */
        RingBuffer* getRingBuffer (CallID call_id);

        /**
         * Get data from the corresponding ringbuffer
         */
        int getDataByID (void *buffer, int toCopy, unsigned short volume, CallID call_id, CallID reader_id);

        /**
         * Return the corresponding data size from a specific ringbuffer
         */
        int availForGetByID (CallID call_id, CallID reader_id);

        /**
         * Discard the specified number of frames from a specific ring buffer
         */
        int discardByID (int toDiscard, CallID call_id, CallID reader_id);

        /**
         * Discard data for this specific ring buffer
         */
        void flushByID (CallID call_id, CallID reader_id);

        RingBufferMap _ringBufferMap;

        CallIDMap _callIDMap;

        SFLDataFormat* mixBuffer;

        ost::Mutex _mutex;

        int _internalSamplingRate;

    public:

        friend class MainBufferTest;
};

#endif
