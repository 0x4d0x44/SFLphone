/*
 *  Copyright (C) 2004, 2005, 2006, 2009, 2008, 2009, 2010, 2011 Savoir-Faire Linux Inc.
 *  Author: Tristan Matthews <tristan.matthews@savoirfairelinux.com>
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

#include "video_rtp_session.h"
#include <cassert>
#include <iostream>
#include <sstream>
#include <map>
#include <string>
#include "video_send_thread.h"
#include "video_receive_thread.h"

namespace sfl_video {

VideoRtpSession::VideoRtpSession(std::map<std::string,std::string> args) :
    args_(args)
{
}

void VideoRtpSession::test()
{
    assert(sendThread_.get() == 0);
    assert(receiveThread_.get() == 0);

    sendThread_.reset(new VideoSendThread(args_));
    sendThread_->start();

    /* block until SDP is ready */
    sendThread_->waitForSDP();
    std::cerr << "SDP File written" << std::endl;
}

void VideoRtpSession::test_loopback()
{
    assert(sendThread_.get() == 0);

    sendThread_.reset(new VideoSendThread(args_));
    sendThread_->start();

    sendThread_->waitForSDP();
    std::map<std::string, std::string> args(args_);
    args["input"] = "test.sdp";

    receiveThread_.reset(new VideoReceiveThread(args));
    receiveThread_->start();
}

void VideoRtpSession::start()
{
    assert(sendThread_.get() == 0);
    assert(receiveThread_.get() == 0);

    sendThread_.reset(new VideoSendThread(args_));
    sendThread_->start();

    sendThread_->waitForSDP();
    std::map<std::string, std::string> args(args_);
    args["input"] = "test.sdp";
    receiveThread_.reset(new VideoReceiveThread(args));
    receiveThread_->start();
}

void VideoRtpSession::stop()
{
    std::cerr << "Stopping video rtp session " << std::endl;
    // FIXME: all kinds of evil!!! interrupted should be atomic
    if (receiveThread_.get())
    {
        receiveThread_->stop();
        receiveThread_->join();
    }

    if (sendThread_.get())
    {
        sendThread_->stop();
        sendThread_->join();
    }
    std::cerr << "cancelled video rtp session " << std::endl;

    // destroy objects
    receiveThread_.reset();
    sendThread_.reset();
}

} // end namspace sfl_video