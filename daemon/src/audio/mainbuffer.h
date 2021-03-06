/*
 *  Copyright (C) 2004, 2005, 2006, 2008, 2009, 2010, 2011 Savoir-Faire Linux Inc.
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

typedef std::map<std::string, RingBuffer*> RingBufferMap;

typedef std::set<std::string> CallIDSet;

typedef std::map<std::string, CallIDSet*> CallIDMap;

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
        void bindCallID (std::string call_id1, std::string call_id2 = default_id);

        /**
         * Add a new call_id to unidirectional outgoing stream
         * \param call_id New call id to be added for this stream
         * \param process_id Process that require this stream
         */
        void bindHalfDuplexOut (std::string process_id, std::string call_id = default_id);

        /**
         * Unbind two calls
         */
        void unBindCallID (std::string call_id1, std::string call_id2 = default_id);

        /**
         * Unbind a unidirectional stream
         */
        void unBindHalfDuplexOut (std::string process_id, std::string call_id = default_id);

        void unBindAll (std::string call_id);

        void unBindAllHalfDuplexOut (std::string process_id);

        void putData (void *buffer, int toCopy, std::string call_id = default_id);

        int getData (void *buffer, int toCopy, std::string call_id = default_id);

        int availForGet (std::string call_id = default_id);

        int discard (int toDiscard, std::string call_id = default_id);

        void flush (std::string call_id = default_id);

        void flushAllBuffers();

        void flushDefault();

        void syncBuffers (std::string call_id);

        void stateInfo();

    private:

        CallIDSet* getCallIDSet (std::string call_id);

        void createCallIDSet (std::string set_id);

        bool removeCallIDSet (std::string set_id);

        /**
         * Add a new call id to this set
         */
        void addCallIDtoSet (std::string set_id, std::string call_id);

        void removeCallIDfromSet (std::string set_id, std::string call_id);

        /**
         * Create a new ringbuffer with default readpointer
         */
        RingBuffer* createRingBuffer (std::string call_id);

        bool removeRingBuffer (std::string call_id);

        RingBuffer* getRingBuffer (std::string call_id);

        int getDataByID (void *buffer, int toCopy, std::string call_id, std::string reader_id);

        int availForGetByID (std::string call_id, std::string reader_id);

        int discardByID (int toDiscard, std::string call_id, std::string reader_id);

        void flushByID (std::string call_id, std::string reader_id);

        RingBufferMap _ringBufferMap;

        CallIDMap _callIDMap;

        ost::Mutex _mutex;

        int _internalSamplingRate;

    public:

        friend class MainBufferTest;
};

#endif
