/*
 *  Copyright (C) 2011 Savoir-Faire Linux Inc.
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

#include "test_video_endpoint.h"
#include <cstdlib>
#include <memory>
#include <iostream>
#include <cassert>
#include "video_endpoint.h"
#include "libav_utils.h"

void VideoEndpointTest::testIsSupportedCodec()
{
    /* This would list codecs */
    assert(libav_utils::isSupportedCodec("mpeg4"));
    assert(not libav_utils::isSupportedCodec("mp3"));
    assert(not libav_utils::isSupportedCodec("xan_wc4"));
    assert(not libav_utils::isSupportedCodec("schroedinger"));
}

void VideoEndpointTest::testListInstalledCodecs()
{
    /* This would list codecs */
    libav_utils::installedCodecs();
}

void VideoEndpointTest::testCodecMap()
{
    /* This would list codecs */
    typedef std::map<int, std::string> MapType;
    const MapType CODECS_MAP(sfl_video::getCodecsMap());
    int count = 0;
    for (MapType::const_iterator iter = CODECS_MAP.begin(); iter != CODECS_MAP.end(); ++iter)
        if (iter->second == "MP4V-ES")
            count++;

    assert(count == 1);
}

int main (int argc, char* argv[])
{
    VideoEndpointTest test;
    test.testListInstalledCodecs();
    test.testCodecMap();
    test.testIsSupportedCodec();
    return 0;
}