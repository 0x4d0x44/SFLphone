/*
 *  Copyright (C) 2006-2007 Savoir-Faire Linux inc.
 *  Author: Alexandre Bourget <alexandre.bourget@savoirfairelinux.com>
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
#include "iaxvoiplink.h"
#include "iaxcall.h"
#include "eventthread.h"
#include "iaxaccount.h"
#include "manager.h"
#include "audio/audiolayer.h"

#include <math.h>
#include <dlfcn.h>

#define IAX_BLOCKING    1
#define IAX_NONBLOCKING 0

#define IAX_SUCCESS  0
#define IAX_FAILURE -1

#define RANDOM_IAX_PORT   rand() % 64000 + 1024

#define MUSIC_ONHOLD true

#define CHK_VALID_CALL   if (call == NULL) { _debug("IAX: Call doesn't exists\n"); \
    return false; }

IAXVoIPLink::IAXVoIPLink (const AccountID& accountID)
        : VoIPLink (accountID)
{
    // _debug("IAXVoIPLink::IAXVoIPLink : creating eventhread \n ");
    _evThread = new EventThread (this);
    _regSession = NULL;
    _nextRefreshStamp = 0;

    // to get random number for RANDOM_PORT
    srand (time (NULL));

    audiolayer = NULL;

    converter = new SamplerateConverter();

    int nbSamplesMax = (int) (converter->getFrequence() * converter->getFramesize() / 1000);

    micData = new SFLDataFormat[nbSamplesMax];
    micDataConverted = new SFLDataFormat[nbSamplesMax];
    micDataEncoded = new unsigned char[nbSamplesMax];

    spkrDataConverted = new SFLDataFormat[nbSamplesMax];
    spkrDataDecoded = new SFLDataFormat[nbSamplesMax];

    urlhook = new UrlHook ();
}


IAXVoIPLink::~IAXVoIPLink()
{
    delete _evThread;
    _evThread = NULL;
    _regSession = NULL; // shall not delete it
    terminate();

    audiolayer = NULL;

    delete converter;

    delete [] micData;
    micData = NULL;
    delete [] micDataConverted;
    micDataConverted = NULL;
    delete [] micDataEncoded;
    micDataEncoded = NULL;

    delete [] spkrDataDecoded;
    spkrDataDecoded = NULL;
    delete [] spkrDataConverted;
    spkrDataConverted = NULL;
}

bool
IAXVoIPLink::init()
{
    // If it was done, don't do it again, until we call terminate()
    if (initDone())
        return false;

    bool returnValue = false;

    // _localAddress = "127.0.0.1";
    // port 0 is default
    //  iax_enable_debug(); have to enable debug when compiling iax...
    int port = IAX_DEFAULT_PORTNO;

    int last_port = 0;

    int nbTry = 3;

    while (port != IAX_FAILURE && nbTry) {
        last_port = port;
        port = iax_init (port);

        if (port < 0) {
            _debug ("IAX Warning: already initialize on port %d\n", last_port);
            port = RANDOM_IAX_PORT;
        } else if (port == IAX_FAILURE) {
            _debug ("IAX Fail to start on port %d", last_port);
            port = RANDOM_IAX_PORT;
        } else {
            _debug ("IAX Info: listening on port %d\n", last_port);
            _localPort = last_port;
            returnValue = true;
            _evThread->start();

            audiolayer = Manager::instance().getAudioDriver();
            break;
        }

        nbTry--;

        initDone (true);
    }

    if (port == IAX_FAILURE || nbTry==0) {
        _debug ("Fail to initialize iax\n");

        initDone (false);
    }

    return returnValue;
}

void
IAXVoIPLink::terminate()
{
    // If it was done, don't do it again, until we call init()
    if (!initDone())
        return;

    // iaxc_shutdown();

    // Hangup all calls
    terminateIAXCall();

    initDone (false);
}

void
IAXVoIPLink::terminateIAXCall()
{
    std::string reason = "Dumped Call";
    ost::MutexLock m (_callMapMutex);
    CallMap::iterator iter = _callMap.begin();
    IAXCall *call;

    while (iter != _callMap.end()) {
        call = dynamic_cast<IAXCall*> (iter->second);

        if (call) {
            _mutexIAX.enterMutex();
            iax_hangup (call->getSession(), (char*) reason.c_str());
            _mutexIAX.leaveMutex();
            call->setSession (NULL);
            delete call;
            call = NULL;
        }

        iter++;
    }

    _callMap.clear();
}

void IAXVoIPLink::terminateOneCall (const CallID& id)
{
    IAXCall* call = getIAXCall (id);

    if (call) {
        _debug ("IAXVoIPLink::terminateOneCall()::the call is deleted, should close recording file \n");
        delete call;
        call = 0;
    }
}



void
IAXVoIPLink::getEvent()
{
    IAXCall* call = NULL;

    // lock iax_ stuff..
    _mutexIAX.enterMutex();
    iax_event* event = NULL;

    while ( (event = iax_get_event (IAX_NONBLOCKING)) != NULL) {
        // If we received an 'ACK', libiax2 tells apps to ignore them.
        if (event->etype == IAX_EVENT_NULL) {
            continue;
        }

        //_debug ("Receive IAX Event: %d (0x%x)\n", event->etype, event->etype);

        call = iaxFindCallBySession (event->session);

        if (call) {
            // We know that call, deal with it
            iaxHandleCallEvent (event, call);
            //_audiocodec = Manager::instance().getCodecDescriptorMap().getCodec( call -> getAudioCodec() );
        } else if (event->session && event->session == _regSession) {
            // This is a registration session, deal with it
            iaxHandleRegReply (event);
        } else {
            // We've got an event before it's associated with any call
            iaxHandlePrecallEvent (event);
        }

        // _debug("IAXVoIPLink::getEvent() : timestamp %i \n",event->ts);

        iax_event_free (event);
    }

    _mutexIAX.leaveMutex();


    if (call) {
        call->recAudio.recData (spkrDataConverted,micData,nbSampleForRec_,nbSampleForRec_);
    }

    // Do the doodle-moodle to send audio from the microphone to the IAX channel.
    sendAudioFromMic();

    // Refresh registration.
    if (_nextRefreshStamp && _nextRefreshStamp - 2 < time (NULL)) {
        sendRegister ("");
    }

    // reinitialize speaker buffer for recording (when recording a voice mail)
    for (int i = 0; i < nbSampleForRec_; i++)
        spkrDataConverted[i] = 0;


    // thread wait 3 millisecond
    _evThread->sleep (3);

    free (event);
}

void
IAXVoIPLink::sendAudioFromMic (void)
{
    // _debug("IAXVoIPLink::sendAudioFromMic");

    int maxBytesToGet, availBytesFromMic, bytesAvail, compSize;
    AudioCodec *ac;
    IAXCall *currentCall;

    // We have to update the audio layer type in case we switched
    // TODO Find out a better way to do it
    updateAudiolayer();

    currentCall = getIAXCall (Manager::instance().getCurrentCallId());

    if (!currentCall) {
        // Let's mind our own business.
        return;
    }

    if (currentCall -> getAudioCodec() < 0)
        return;

    // Just make sure the currentCall is in state to receive audio right now.
    //_debug("Here we get: connectionState: %d   state: %d \n",
    //currentCall->getConnectionState(),
    //currentCall->getState());

    if (currentCall->getConnectionState() != Call::Connected ||
            currentCall->getState() != Call::Active) {
        return;
    }

    ac = currentCall->getCodecMap().getCodec (currentCall -> getAudioCodec());

    if (!ac) {
        // Audio codec still not determined.
        if (audiolayer) {
            // To keep latency low..
            audiolayer->flushMic();
        }

        return;
    }

    // Send sound here
    if (audiolayer) {

        // we have to get 20ms of data from the mic *20/1000 = /50
        // rate/50 shall be lower than IAX__20S_48KHZ_MAX
        maxBytesToGet = audiolayer->getSampleRate() * audiolayer->getFrameSize() / 1000 * sizeof (SFLDataFormat);

        // available bytes inside ringbuffer
        availBytesFromMic = audiolayer->canGetMic();

        if (availBytesFromMic < maxBytesToGet) {
            // We need packets full!
            return;
        }

        // take the lowest
        bytesAvail = (availBytesFromMic < maxBytesToGet) ? availBytesFromMic : maxBytesToGet;

        //_debug("available = %d, maxBytesToGet = %d\n", availBytesFromMic, maxBytesToGet);

        // Get bytes from micRingBuffer to data_from_mic
        nbSample_ = audiolayer->getMic (micData, bytesAvail) / sizeof (SFLDataFormat);

        // Store the number of samples for recording
        nbSampleForRec_ = nbSample_;

        // _debug("IAXVoIPLink::sendAudioFromMic : %i \n",nbSampleForRec_);

        // resample
        nbSample_ = converter->downsampleData (micData , micDataConverted , (int) ac ->getClockRate() , (int) audiolayer->getSampleRate() , nbSample_);

        // for the mono: range = 0 to IAX_FRAME2SEND * sizeof(int16)
        compSize = ac->codecEncode (micDataEncoded, micDataConverted , nbSample_*sizeof (int16));

        // Send it out!
        _mutexIAX.enterMutex();

        // Make sure the session and the call still exists.
        if (currentCall->getSession() && micDataEncoded != NULL) {
            if (iax_send_voice (currentCall->getSession(), currentCall->getFormat(), micDataEncoded, compSize, nbSample_) == -1) {
                _debug ("IAX: Error sending voice data.\n");
            }
        }

        _mutexIAX.leaveMutex();
    }
}


IAXCall*
IAXVoIPLink::getIAXCall (const CallID& id)
{
    Call* call = getCall (id);

    if (call) {
        return dynamic_cast<IAXCall*> (call);
    }

    return NULL;
}


int
IAXVoIPLink::sendRegister (AccountID id)
{
    IAXAccount *account;
    bool result;

    result = false;
    account = dynamic_cast<IAXAccount *> (getAccountPtr());

    if (account->getHostname().empty()) {
        return false;
    }

    if (account->getUsername().empty()) {
        return false;
    }

    // lock
    _mutexIAX.enterMutex();

    // Always use a brand new session
    if (_regSession) {
        iax_destroy (_regSession);
    }

    _regSession = iax_session_new();

    if (!_regSession) {
        _debug ("Error when generating new session for register");
    } else {
        _debug ("IAX Sending registration to %s with user %s\n", account->getHostname().c_str() , account->getUsername().c_str());
        int val = iax_register (_regSession, account->getHostname().data(), account->getUsername().data(), account->getPassword().data(), 120);
        _debug ("Return value: %d\n", val);
        // set the time-out to 15 seconds, after that, resend a registration request.
        // until we unregister.
        _nextRefreshStamp = time (NULL) + 10;
        result = true;

        account->setRegistrationState (Trying);
    }

    // unlock
    _mutexIAX.leaveMutex();

    return result;
}

int
IAXVoIPLink::sendUnregister (AccountID id)
{
    IAXAccount *account;

    account = dynamic_cast<IAXAccount*> (getAccountPtr());

    if (!account)
        return 1;

    _mutexIAX.enterMutex();

    if (_regSession) {
        /** @todo Should send a REGREL in sendUnregister()... */
        //iax_send_regrel(); doesn't exist yet :)
        iax_destroy (_regSession);
        _regSession = NULL;
    }

    _mutexIAX.leaveMutex();

    _nextRefreshStamp = 0;

    _debug ("IAX2 send unregister\n");
    account->setRegistrationState (Unregistered);

    return SUCCESS;
}

Call*
IAXVoIPLink::newOutgoingCall (const CallID& id, const std::string& toUrl)
{
    IAXCall* call = new IAXCall (id, Call::Outgoing);
    call->setCodecMap (Manager::instance().getCodecDescriptorMap());

    if (call) {
        call->setPeerNumber (toUrl);
        call->initRecFileName();

        if (iaxOutgoingInvite (call)) {
            call->setConnectionState (Call::Progressing);
            call->setState (Call::Active);
            addCall (call);
        } else {
            delete call;
            call = NULL;
        }
    }

    return call;
}


bool
IAXVoIPLink::answer (const CallID& id)
{
    IAXCall* call = getIAXCall (id);
    call->setCodecMap (Manager::instance().getCodecDescriptorMap());

    CHK_VALID_CALL;

    _mutexIAX.enterMutex();
    iax_answer (call->getSession());
    _mutexIAX.leaveMutex();

    call->setState (Call::Active);
    call->setConnectionState (Call::Connected);
    // Start audio
    audiolayer->startStream();

    return true;
}

bool
IAXVoIPLink::hangup (const CallID& id)
{
    _debug ("IAXVoIPLink::hangup() : function called once hangup \n");
    IAXCall* call = getIAXCall (id);
    std::string reason = "Dumped Call";
    CHK_VALID_CALL;
    _mutexIAX.enterMutex();

    iax_hangup (call->getSession(), (char*) reason.c_str());
    _mutexIAX.leaveMutex();
    call->setSession (NULL);

    if (Manager::instance().isCurrentCall (id)) {
        // stop audio
        audiolayer->stopStream();
    }

    terminateOneCall (id);

    removeCall (id);
    return true;
}


bool
IAXVoIPLink::peerHungup (const CallID& id)
{
    _debug ("IAXVoIPLink::peerHangup() : function called once hangup \n");
    IAXCall* call = getIAXCall (id);
    std::string reason = "Dumped Call";
    CHK_VALID_CALL;
    _mutexIAX.enterMutex();

    _mutexIAX.leaveMutex();
    call->setSession (NULL);

    if (Manager::instance().isCurrentCall (id)) {
        // stop audio
        audiolayer->stopStream();
    }

    terminateOneCall (id);

    removeCall (id);
    return true;
}



bool
IAXVoIPLink::onhold (const CallID& id)
{
    IAXCall* call = getIAXCall (id);

    CHK_VALID_CALL;

    //if (call->getState() == Call::Hold) { _debug("Call is already on hold\n"); return false; }

    _mutexIAX.enterMutex();
    iax_quelch_moh (call->getSession() , MUSIC_ONHOLD);
    _mutexIAX.leaveMutex();

    call->setState (Call::Hold);
    return true;
}

bool
IAXVoIPLink::offhold (const CallID& id)
{
    IAXCall* call = getIAXCall (id);

    CHK_VALID_CALL;

    //if (call->getState() == Call::Active) { _debug("Call is already active\n"); return false; }
    _mutexIAX.enterMutex();
    iax_unquelch (call->getSession());
    _mutexIAX.leaveMutex();
    audiolayer->startStream();
    call->setState (Call::Active);
    return true;
}

bool
IAXVoIPLink::transfer (const CallID& id, const std::string& to)
{
    IAXCall* call = getIAXCall (id);

    CHK_VALID_CALL;

    char callto[to.length() +1];
    strcpy (callto, to.c_str());

    _mutexIAX.enterMutex();
    iax_transfer (call->getSession(), callto);
    _mutexIAX.leaveMutex();

    return true;

    // should we remove it?
    // removeCall(id);
}

bool
IAXVoIPLink::refuse (const CallID& id)
{
    IAXCall* call = getIAXCall (id);
    std::string reason = "Call rejected manually.";

    CHK_VALID_CALL;

    _mutexIAX.enterMutex();
    iax_reject (call->getSession(), (char*) reason.c_str());
    _mutexIAX.leaveMutex();


    // terminateOneCall(id);
    removeCall (id);

    return true;
}


void
IAXVoIPLink::setRecording (const CallID& id)
{
    _debug ("IAXVoIPLink::setRecording()!");

    IAXCall* call = getIAXCall (id);

    call->setRecording();
}

bool
IAXVoIPLink::isRecording (const CallID& id)
{
    _debug ("IAXVoIPLink::setRecording()!");

    IAXCall* call = getIAXCall (id);

    return call->isRecording();
}




bool
IAXVoIPLink::carryingDTMFdigits (const CallID& id, char code)
{
    IAXCall* call = getIAXCall (id);

    CHK_VALID_CALL;

    _mutexIAX.enterMutex();
    iax_send_dtmf (call->getSession(), code);
    _mutexIAX.leaveMutex();

    return true;
}


std::string
IAXVoIPLink::getCurrentCodecName()
{
    IAXCall *call = getIAXCall (Manager::instance().getCurrentCallId());

    AudioCodec *ac = call->getCodecMap().getCodec (call->getAudioCodec());

    return ac->getCodecName();
}


bool
IAXVoIPLink::iaxOutgoingInvite (IAXCall* call)
{

    struct iax_session *newsession;
    ost::MutexLock m (_mutexIAX);
    std::string username, strNum;
    char *lang=NULL;
    int wait, audio_format_preferred, audio_format_capability;
    IAXAccount *account;

    newsession = iax_session_new();

    if (!newsession) {
        _debug ("IAX Error: Can't make new session for a new call\n");
        return false;
    }

    call->setSession (newsession);

    account = dynamic_cast<IAXAccount*> (getAccountPtr());
    username = account->getUsername();
    strNum = username + ":" + account->getPassword() + "@" + account->getHostname() + "/" + call->getPeerNumber();

    wait = 0;
    /** @todo Make preference dynamic, and configurable */
    audio_format_preferred =  call->getFirstMatchingFormat (call->getSupportedFormat());
    audio_format_capability = call->getSupportedFormat();

    _debug ("IAX New call: %s\n", strNum.c_str());
    iax_call (newsession, username.c_str(), username.c_str(), strNum.c_str(), lang, wait, audio_format_preferred, audio_format_capability);

    return true;
}


IAXCall*
IAXVoIPLink::iaxFindCallBySession (struct iax_session* session)
{
    // access to callMap shoud use that
    // the code below is like findSIPCallWithCid()
    ost::MutexLock m (_callMapMutex);
    IAXCall* call = NULL;
    CallMap::iterator iter = _callMap.begin();

    while (iter != _callMap.end()) {
        call = dynamic_cast<IAXCall*> (iter->second);

        if (call && call->getSession() == session) {
            return call;
        }

        iter++;
    }

    return NULL; // not found
}

void
IAXVoIPLink::iaxHandleCallEvent (iax_event* event, IAXCall* call)
{
    // call should not be 0
    // note activity?
    //
    CallID id = call->getCallId();

    switch (event->etype) {

        case IAX_EVENT_HANGUP:

            if (Manager::instance().isCurrentCall (id)) {
                // stop audio
                audiolayer->stopStream();
            }

            Manager::instance().peerHungupCall (id);

            /*
            _debug("IAXVoIPLink::iaxHandleCallEvent, peer hangup have been called");
            std::string reason = "Dumped Call";
            _mutexIAX.enterMutex();
            iax_hangup(call->getSession(), (char*)reason.c_str());
            _mutexIAX.leaveMutex();
            call->setSession(NULL);
            audiolayer->stopStream();
            terminateOneCall(id);
            */
            removeCall (id);
            break;

        case IAX_EVENT_REJECT:
            //Manager::instance().peerHungupCall(id);

            if (Manager::instance().isCurrentCall (id)) {
                // stop audio
                audiolayer->stopStream();
            }

            call->setConnectionState (Call::Connected);

            call->setState (Call::Error);
            Manager::instance().callFailure (id);
            // terminateOneCall(id);
            removeCall (id);
            break;

        case IAX_EVENT_ACCEPT:
            // Call accepted over there by the computer, not the user yet.

            if (event->ies.format) {
                call->setFormat (event->ies.format);
            }

            break;

        case IAX_EVENT_ANSWER:

            if (call->getConnectionState() != Call::Connected) {
                call->setConnectionState (Call::Connected);
                call->setState (Call::Active);
                audiolayer->startStream();

                if (event->ies.format) {
                    // Should not get here, should have been set in EVENT_ACCEPT
                    call->setFormat (event->ies.format);
                }

                Manager::instance().peerAnsweredCall (id);

                // start audio here?
            } else {
                // deja connecté ?
            }

            break;

        case IAX_EVENT_BUSY:
            call->setConnectionState (Call::Connected);
            call->setState (Call::Busy);
            Manager::instance().callBusy (id);
            // terminateOneCall(id);
            removeCall (id);
            break;

        case IAX_EVENT_VOICE:
            //if (!audiolayer->isCaptureActive ())
            //  audiolayer->startStream ();
            // _debug("IAX_EVENT_VOICE: \n");
            iaxHandleVoiceEvent (event, call);
            break;

        case IAX_EVENT_TEXT:
            break;

        case IAX_EVENT_RINGA:
            call->setConnectionState (Call::Ringing);
            Manager::instance().peerRingingCall (call->getCallId());
            break;

        case IAX_IE_MSGCOUNT:
            break;

        case IAX_EVENT_PONG:
            break;

        case IAX_EVENT_URL:

            if (Manager::instance().getConfigString (HOOKS, URLHOOK_IAX2_ENABLED) == "1") {
                if (strcmp ( (char*) event->data, "") != 0) {
                    _debug ("> IAX_EVENT_URL received: %s\n", event->data);
                    urlhook->addAction ( (char*) event->data, Manager::instance().getConfigString (HOOKS, URLHOOK_COMMAND));
                }
            }

            break;

        case IAX_EVENT_TIMEOUT:
            break;

        case IAX_EVENT_TRANSFER:
            break;

        default:
            _debug ("iaxHandleCallEvent: Unknown event type (in call event): %d\n", event->etype);

    }
}


/* Handle audio event, VOICE packet received */
void
IAXVoIPLink::iaxHandleVoiceEvent (iax_event* event, IAXCall* call)
{

    unsigned char *data;
    unsigned int size, max, nbInt16;
    int expandedSize, nbSample_;
    AudioCodec *ac;

    // If we receive datalen == 0, some things of the jitter buffer in libiax2/iax.c
    // were triggered

    if (!event->datalen) {
        // Skip this empty packet.
        //_debug("IAX: Skipping empty jitter-buffer interpolated packet\n");
        return;
    }

    if (audiolayer) {
        // On-the-fly codec changing (normally, when we receive a full packet)
        // as per http://tools.ietf.org/id/draft-guy-iax-03.txt
        // - subclass holds the voiceformat property.
        if (event->subclass && event->subclass != call->getFormat()) {
            call->setFormat (event->subclass);
        }

        //_debug("Receive: len=%d, format=%d, _receiveDataDecoded=%p\n", event->datalen, call->getFormat(), _receiveDataDecoded);
        ac = call->getCodecMap().getCodec (call -> getAudioCodec());

        data = (unsigned char*) event->data;

        size   = event->datalen;

        // Decode data with relevant codec
        max = (int) (ac->getClockRate() * audiolayer->getFrameSize() / 1000);

        if (size > max) {
            _debug ("The size %d is bigger than expected %d. Packet cropped. Ouch!\n", size, max);
            size = max;
        }

        expandedSize = ac->codecDecode (spkrDataDecoded , data , size);

        nbInt16      = expandedSize/sizeof (int16);

        if (nbInt16 > max) {
            _debug ("We have decoded an IAX VOICE packet larger than expected: %i VS %i. Cropping.\n", nbInt16, max);
            nbInt16 = max;
        }

        nbSample_ = nbInt16;

        // resample
        nbInt16 = converter->upsampleData (spkrDataDecoded , spkrDataConverted , ac->getClockRate() , audiolayer->getSampleRate() , nbSample_);

        /* Write the data to the mic ring buffer */
        audiolayer->putMain (spkrDataConverted , nbInt16 * sizeof (SFLDataFormat));

    } else {
        _debug ("IAX: incoming audio, but no sound card open");
    }

}

/**
 * Handle the registration process
 */
void
IAXVoIPLink::iaxHandleRegReply (iax_event* event)
{

    std::string account_id;
    IAXAccount *account;

    account_id = getAccountID();
    account = dynamic_cast<IAXAccount *> (Manager::instance().getAccount (account_id));

    if (event->etype == IAX_EVENT_REGREJ) {
        /* Authentication failed! */
        _mutexIAX.enterMutex();
        iax_destroy (_regSession);
        _mutexIAX.leaveMutex();
        _regSession = NULL;
        // Update the account registration state
        account->setRegistrationState (ErrorAuth);
    }

    else if (event->etype == IAX_EVENT_REGACK) {
        /* Authentication succeeded */
        _mutexIAX.enterMutex();

        // Looking for the voicemail information
        //if( event->ies != 0 )
        //new_voicemails = processIAXMsgCount(event->ies.msgcount);
        //_debug("iax voicemail number notification: %i\n", new_voicemails);
        // Notify the client if new voicemail waiting for the current account
        //account_id = getAccountID();
        //Manager::instance().startVoiceMessageNotification(account_id.c_str(), new_voicemails);

        iax_destroy (_regSession);
        _mutexIAX.leaveMutex();
        _regSession = NULL;

        // I mean, save the timestamp, so that we re-register again in the REFRESH time.
        // Defaults to 60, as per draft-guy-iax-03.
        _nextRefreshStamp = time (NULL) + (event->ies.refresh ? event->ies.refresh : 60);
        account->setRegistrationState (Registered);
    }
}

int IAXVoIPLink::processIAXMsgCount (int msgcount)
{

    // IAX sends the message count under a specific format:
    //                       1
    //  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5
    //  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    //  |      0x18     |      0x02     |
    //  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    //  |  Old messages |  New messages |
    //  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

    // For now we just need the new messages informations.
    // Thus:
    // 0 <= msgcount <= 255   => msgcount new messages, 0 old messages
    // msgcount >= 256        => msgcount/256 old messages , msgcount%256 new messages (RULES)

    return msgcount%256;

}


void
IAXVoIPLink::iaxHandlePrecallEvent (iax_event* event)
{
    IAXCall* call = NULL;
    CallID   id;
    std::string reason = "Error ringing user.";

    switch (event->etype) {

        case IAX_EVENT_REGACK:

        case IAX_EVENT_REGREJ:
            _debug ("IAX Registration Event in a pre-call setup\n");
            break;

        case IAX_EVENT_REGREQ:
            // Received when someone wants to register to us!?!
            // Asterisk receives and answers to that, not us, we're a phone.
            _debug ("Registration by a peer, don't allow it\n");
            break;

        case IAX_EVENT_CONNECT:
            // We've got an incoming call! Yikes!
            _debug ("> IAX_EVENT_CONNECT (receive)\n");

            id = Manager::instance().getNewCallID();

            call = new IAXCall (id, Call::Incoming);

            if (!call) {
                _debug ("! IAX Failure: unable to create an incoming call");
                return;
            }

            // Setup the new IAXCall
            // Associate the call to the session.
            call->setSession (event->session);

            // setCallAudioLocal(call);
            call->setCodecMap (Manager::instance().getCodecDescriptorMap());

            call->setConnectionState (Call::Progressing);


            if (event->ies.calling_number)
                call->setPeerNumber (std::string (event->ies.calling_number));

            if (event->ies.calling_name)
                call->setPeerName (std::string (event->ies.calling_name));

            // if peerNumber exist append it to the name string
            call->initRecFileName();

            if (Manager::instance().incomingCall (call, getAccountID())) {
                /** @todo Faudra considérer éventuellement le champ CODEC PREFS pour
                 * l'établissement du codec de transmission */

                // Remote lists its capabilities
                int format = call->getFirstMatchingFormat (event->ies.capability);
                // Remote asks for preferred codec voiceformat
                int pref_format = call->getFirstMatchingFormat (event->ies.format);

                // Priority to remote's suggestion. In case it's a forwarding, no transcoding
                // will be needed from the server, thus less latency.

                if (pref_format)
                    format = pref_format;

                iax_accept (event->session, format);

                iax_ring_announce (event->session);

                addCall (call);
            } else {
                // reject call, unable to add it
                iax_reject (event->session, (char*) reason.c_str());

                delete call;
                call = NULL;
            }

            break;

        case IAX_EVENT_HANGUP:
            // Remote peer hung up
            call = iaxFindCallBySession (event->session);
            id = call->getCallId();
            _debug ("IAXVoIPLink::hungup::iaxHandlePrecallEvent");
            Manager::instance().peerHungupCall (id);
            // terminateOneCall(id);
            removeCall (id);
            break;

        case IAX_EVENT_TIMEOUT: // timeout for an unknown session

            break;

        case IAX_IE_MSGCOUNT:
            //_debug("messssssssssssssssssssssssssssssssssssssssssssssssages\n");
            break;

        default:
            _debug ("IAXVoIPLink::iaxHandlePrecallEvent: Unknown event type (in precall): %d\n", event->etype);
    }

}

void IAXVoIPLink::updateAudiolayer (void)
{
    _mutexIAX.enterMutex();
    audiolayer = NULL;
    audiolayer = Manager::instance().getAudioDriver();
    _mutexIAX.leaveMutex();
}

