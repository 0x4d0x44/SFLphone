/*
 *  Copyright (C) 2009 Savoir-Faire Linux inc.
 *
 *  Author: Emmanuel Milou <emmanuel.milou@savoirfairelinux.com>
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

#ifndef _SDP_H
#define _SDP_H

#include <pjmedia/sdp.h>
#include <pjmedia/sdp_neg.h>
#include <pjsip/sip_transport.h>
#include <pjlib.h>
#include <pj/pool.h>
#include <pj/assert.h>
#include <exception>

#include "audio/codecDescriptor.h"
#include "sdpmedia.h"

class sdpException: public std::exception
{
  virtual const char* what() const throw()
  {
    return "An sdpException Occured";
  }
};


class Sdp {

    public:
        
        /*
         * Class Constructor.
         *
         * @param ip_addr
         */
        Sdp(pj_pool_t *pool);

        /* Class destructor */
        ~Sdp();

        /*
         * Read accessor. Get the list of the local media capabilities. 
         *
         * @return std::vector<sdpMedia*>   the vector containing the different media
         */
        std::vector<sdpMedia*> get_local_media_cap( void ) { return _local_media_cap; }

         /*
         *  Read accessor. Get the sdp session information
         *
         *  @return pjmedia_sdp_session   The structure that describes a SDP session
         */
        pjmedia_sdp_session* get_local_sdp_session( void ) { return _local_offer; }

        /*
         * Write accessor. Set the local IP address that will be used in the sdp session
         */
        void set_ip_address( std::string ip_addr ) { _ip_addr = ip_addr; }

        /*
         * Read accessor. Get the local IP address
         */
        std::string get_ip_address( void ) { return _ip_addr; }
        
        /*
         * Build the local SDP offer
         */
        int create_local_offer( );

        /*
         * Build the sdp media section
         * Add rtpmap field if necessary
         *
         * @param media     The media to add to SDP
         * @param med   The structure to receive the media section
         */
        void set_media_descriptor_line( sdpMedia* media, pjmedia_sdp_media** p_med );

        /*
         * On building an invite outside a dialog, build the local offer and create the
         * SDP negociator instance with it.
         */
        int create_initial_offer( );

         /*
         * On receiving an invite outside a dialog, build the local offer and create the
         * SDP negociator instance with the remote offer.
         *
         * @param remote    The remote offer
         */
        int receiving_initial_offer( pjmedia_sdp_session* remote );
        
        /*
         * Remove all media in the session media vector.
         */
        void clean_session_media();

        /*
         * Return a string description of the media added to the session,
         * ie the local media capabilities
         */
        std::string media_to_string( void );

        /*
         * Return the codec of the first media after negociation
         */
        AudioCodec* get_session_media( void );

        /*
         * read accessor. Return the negociated offer
         *
         * @return pjmedia_sdp_session  The negociated offer
         */
        pjmedia_sdp_session* get_negociated_offer( void ){
            return _negociated_offer;
        }

         /*
         * Start the sdp negociation.
         *
         * @return pj_status_t  0 on success
         *                      1 otherwise
         */
        pj_status_t start_negociation( void ){
            return pjmedia_sdp_neg_negotiate(
                       _pool, _negociator, 0);
        }

         /*
         * Retrieve the negociated sdp offer from the sip payload.
         *
         * @param sdp   the negociated offer
         */
        void set_negociated_offer( const pjmedia_sdp_session *sdp );

        /*
         * Attribute the specified port to every medias provided
         * This is valid only because we are using one media
         * We should change this to support multiple medias
         *
         * @param port  The media port
         */
        void attribute_port_to_all_media (int port);

        void  set_local_extern_audio_port(int port){ _local_extern_audio_port = port; }

        int  get_local_extern_audio_port (void){ return _local_extern_audio_port; }

        void toString (void);

        /** 
         * Set remote's IP addr. [not protected]
         * @param ip  The remote IP address
         */
        void set_remote_ip(const std::string& ip)    { _remote_ip_addr = ip; }
        
        /** 
         * Return IP of destination [mutex protected]
         * @return const std:string	The remote IP address
         */
        const std::string& get_remote_ip() { return _remote_ip_addr; }

        /** 
         * Set remote's audio port. [not protected]
         * @param port  The remote audio port
         */
        void set_remote_audio_port(unsigned int port) { _remote_audio_port = port; }

        /** 
         * Return audio port at destination [mutex protected] 
         * @return unsigned int The remote audio port
         */
        unsigned int get_remote_audio_port() { return _remote_audio_port; }

        void fetch_media_transport_info_from_remote_sdp (pjmedia_sdp_session *remote_sdp);

        std::vector<sdpMedia*> get_session_media_list (void) { return _session_media; }

    private:
        /** Codec Map */
        std::vector<sdpMedia*> _local_media_cap;

        /* The media that will be used by the session (after the SDP negociation) */
        std::vector<sdpMedia*> _session_media;

        /** negociator */
        pjmedia_sdp_neg *_negociator;

        /** IP address */
        std::string _ip_addr;

        /** Remote's IP address */
        std::string  _remote_ip_addr;
        
        /** Local SDP */
        pjmedia_sdp_session *_local_offer;

        /* The negociated SDP offer */
        // Explanation: each endpoint's offer is negociated, and a new sdp offer results from this
        // negociation, with the compatible media from each part 
        pjmedia_sdp_session *_negociated_offer;

        /** Local audio port */
        int _local_extern_audio_port;

        /** Remote's audio port */
        unsigned int _remote_audio_port;

        // The pool to allocate memory
        pj_pool_t *_pool;

        Sdp(const Sdp&); //No Copy Constructor
        Sdp& operator=(const Sdp&); //No Assignment Operator

        void set_local_media_capabilities ();

        /*
         *  Mandatory field: Origin ("o=")
         *  Gives the originator of the session.
         *  Serves as a globally unique identifier for this version of this session description.
         */
        void sdp_add_origin( void );

        /*
         *  Mandatory field: Protocol version ("v=")
         *  Add the protocol version in the SDP session description
         */
        void sdp_add_protocol( void );

        /*
         *  Optional field: Connection data ("c=")
         *  Contains connection data.
         */
        void sdp_add_connection_info( void );
        
        /*
         *  Mandatory field: Session name ("s=")
         *  Add a textual session name.
         */
        void sdp_add_session_name( void );

        /*
         *  Optional field: Session information ("s=")
         *  Provides textual information about the session.
         */
        void sdp_add_session_info( void ){}

        /*
         *  Optional field: Uri ("u=")
         *  Add a pointer to additional information about the session.
         */
        void sdp_add_uri( void ) {}

        /*
         *  Optional fields: Email address and phone number ("e=" and "p=")
         *  Add contact information for the person responsible for the conference.
         */
        void sdp_add_email( void ) {}

        /*
         *  Optional field: Bandwidth ("b=")
         *  Denotes the proposed bandwidth to be used by the session or the media .
         */
        void sdp_add_bandwidth( void ) {}

        /*
         *  Mandatory field: Timing ("t=")
         *  Specify the start and the stop time for a session.
         */
        void sdp_add_timing( void );

        /*
         * Optional field: Time zones ("z=")
         */
        void sdp_add_time_zone( void ) {}

        /*
         * Optional field: Encryption keys ("k=")
         */
        void sdp_add_encryption_key( void ) {}

        /*
         * Optional field: Attributes ("a=")
         */
        void sdp_add_attributes( );

        /** 
         * Adds a new sdp attribute with given name and value
         *
         */
        void sdp_add_new_attribute(std::string& name, std::string& value);
        
        /*
         * Mandatory field: Media descriptions ("m=")
         */
        void sdp_add_media_description();

        std::string convert_int_to_string (int value);

        void fetch_remote_ip_from_sdp (pjmedia_sdp_session *r_sdp);
        
        void fetch_remote_audio_port_from_sdp (pjmedia_sdp_media *r_media);

        void get_remote_sdp_media_from_offer (pjmedia_sdp_session* r_sdp, pjmedia_sdp_media** r_media);

//////////////////////////////////////////////////////////////////3
////////////////////////////////////////////////////////////////////
              
};


#endif
