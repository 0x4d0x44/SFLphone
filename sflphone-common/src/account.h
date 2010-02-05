/*
 *  Copyright (C) 2006-2009 Savoir-Faire Linux inc.
 *  Author: Emmanuel Milou <emmanuel.milou@savoirfairelinux.com>
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

#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <string>
#include <vector>

#include "config/config.h"
#include "voiplink.h"

class VoIPLink;

/**
 * @file account.h
 * @brief Interface to protocol account (SIPAccount, IAXAccount)
 * It can be enable on loading or activate after.
 * It contains account, configuration, VoIP Link and Calls (inside the VoIPLink)
 */

typedef std::string AccountID;

/** Contains all the state an Voip can be in */
typedef enum RegistrationState {
        Unregistered, 
        Trying, 
        Registered, 
        Error, 
        ErrorAuth , 
        ErrorNetwork , 
        ErrorHost, 
        ErrorExistStun, 
        ErrorConfStun,
        NumberOfState
} RegistrationState;

#define AccountNULL ""

// Account identifier                       
#define ACCOUNT_ID                          "Account.id"

// Common account parameters
#define CONFIG_ACCOUNT_TYPE                 "Account.type"  
#define CONFIG_ACCOUNT_ALIAS                "Account.alias"
#define CONFIG_ACCOUNT_MAILBOX	            "Account.mailbox"
#define CONFIG_ACCOUNT_ENABLE               "Account.enable"
#define CONFIG_ACCOUNT_RESOLVE_ONCE         "Account.resolveOnce"
#define CONFIG_ACCOUNT_REGISTRATION_EXPIRE  "Account.expire"
#define CONFIG_CREDENTIAL_NUMBER            "Credential.count"

#define HOSTNAME                            "hostname"
#define USERNAME                            "username"
#define AUTHENTICATION_USERNAME             "authenticationUsername"
#define PASSWORD                            "password"
#define REALM                               "realm"
#define DEFAULT_REALM                       "*"

#define LOCAL_INTERFACE                     "Account.localInterface"
#define PUBLISHED_SAMEAS_LOCAL              "Account.publishedSameAsLocal"
#define LOCAL_PORT                          "Account.localPort"
#define PUBLISHED_PORT                      "Account.publishedPort"
#define PUBLISHED_ADDRESS                   "Account.publishedAddress"

#define DISPLAY_NAME                        "Account.displayName"
#define DEFAULT_ADDRESS                     "0.0.0.0"

// SIP specific parameters
#define SIP_PROXY                           "SIP.proxy"
#define STUN_SERVER							"STUN.server"
#define STUN_ENABLE							"STUN.enable"

// SRTP specific parameters
#define SRTP_ENABLE                         "SRTP.enable"
#define SRTP_KEY_EXCHANGE                   "SRTP.keyExchange"
#define SRTP_ENCRYPTION_ALGO                "SRTP.encryptionAlgorithm"  // Provided by ccRTP,0=NULL,1=AESCM,2=AESF8 
#define SRTP_RTP_FALLBACK                   "SRTP.rtpFallback"
#define ZRTP_HELLO_HASH                     "ZRTP.helloHashEnable"
#define ZRTP_DISPLAY_SAS                    "ZRTP.displaySAS"
#define ZRTP_NOT_SUPP_WARNING               "ZRTP.notSuppWarning"
#define ZRTP_DISPLAY_SAS_ONCE               "ZRTP.displaySasOnce"

#define TLS_LISTENER_PORT                   "TLS.listenerPort"
#define TLS_ENABLE                          "TLS.enable"
#define TLS_CA_LIST_FILE                    "TLS.certificateListFile"
#define TLS_CERTIFICATE_FILE                "TLS.certificateFile"
#define TLS_PRIVATE_KEY_FILE                "TLS.privateKeyFile"
#define TLS_PASSWORD                        "TLS.password"
#define TLS_METHOD                          "TLS.method"
#define TLS_CIPHERS                         "TLS.ciphers"
#define TLS_SERVER_NAME                     "TLS.serverName"
#define TLS_VERIFY_SERVER                   "TLS.verifyServer"
#define TLS_VERIFY_CLIENT                   "TLS.verifyClient"
#define TLS_REQUIRE_CLIENT_CERTIFICATE      "TLS.requireClientCertificate"  
#define TLS_NEGOTIATION_TIMEOUT_SEC         "TLS.negotiationTimeoutSec"
#define TLS_NEGOTIATION_TIMEOUT_MSEC        "TLS.negotiationTimemoutMsec"

#define REGISTRATION_STATUS                 "Status"
#define REGISTRATION_STATE_CODE             "Registration.code" 
#define REGISTRATION_STATE_DESCRIPTION      "Registration.description"

class Account{

    public:

        Account(const AccountID& accountID, std::string type);

        /**
         * Virtual destructor
         */
        virtual ~Account();

        /**
         * Load the settings for this account.
         */
        virtual void loadConfig();

        /**
         * Get the account ID
         * @return constant account id
         */
        inline const AccountID& getAccountID() { return _accountID; }

        /**
         * Get the voiplink pointer
         * @return VoIPLink* the pointer or 0
         */
        inline VoIPLink* getVoIPLink() { return _link; }

        virtual void setVoIPLink () = 0;

        /**
         * Register the underlying VoIPLink. Launch the event listener.
         * This should update the getRegistrationState() return value.
         */
        virtual int registerVoIPLink() = 0;

        /**
         * Unregister the underlying VoIPLink. Stop the event listener.
         * This should update the getRegistrationState() return value.
         */
        virtual int unregisterVoIPLink() = 0;

        /**
         * Tell if the account is enable or not. 
         * @return true if enabled
         *	     false otherwise
         */
        bool isEnabled() { return _enabled; }

        /**
         * Get the registration state of the specified link
         * @return RegistrationState	The registration state of underlying VoIPLink
         */
        inline RegistrationState getRegistrationState() { return _registrationState; }

        /**
         * Set the registration state of the specified link
         * @param state	The registration state of underlying VoIPLink
         */
        void setRegistrationState( RegistrationState state );
        
        /**
         * Set the latest up-to-date state code
         * for that account. These codes are 
         * those used in SIP and IAX (eg. 200, 500 ...)
         * @param state The Code:Description state
         * @return void
         */
        void setRegistrationStateDetailed(std::pair<int, std::string> state) { _registrationStateDetailed = state; }
        
        /**
         * Get the latest up-to-date state code
         * for that account. These codes are 
         * those used in SIP and IAX (eg. 200, 500 ...)
         * @param void
         * @return std::pair<int, std::string> A Code:Description state
         */
        std::pair<int, std::string> getRegistrationStateDetailed(void) { return _registrationStateDetailed; }
                        

        /* inline functions */
        /* They should be treated like macro definitions by the C++ compiler */
        inline std::string getUsername( void ) { return _username; }
        inline void setUsername( std::string username) { _username = username; }

        inline std::string getHostname( void ) { return _hostname; }
        inline void setHostname( std::string hostname) { _hostname = hostname; }

        inline std::string getPassword( void ) { return _password; }
        inline void setPassword( std::string password ) { _password = password; }

        inline std::string getAlias( void ) { return _alias; }
        inline void setAlias( std::string alias ) { _alias = alias; }

        inline std::string getType( void ) { return _type; }
        inline void setType( std::string type ) { _type = type; }

    private:
        // copy constructor
        Account(const Account& rh);

        // assignment operator
        Account& operator=(const Account& rh);

    protected:
        /**
         * Account ID are assign in constructor and shall not changed
         */
        AccountID _accountID;

        /**
         * Account login information: username
         */
        std::string _username;

        /**
         * Account login information: hostname
         */
        std::string _hostname;

        /**
         * Account login information: password
         */
        std::string _password;

        /**
         * Account login information: Alias
         */
        std::string _alias;

        /**
         * Voice over IP Link contains a listener thread and calls
         */
        VoIPLink* _link;

        /**
         * Tells if the link is enabled, active.
         * This implies the link will be initialized on startup.
         * Modified by the configuration (key: ENABLED)
         */
        bool _enabled;

        /*
         * The account type
         * IAX2 or SIP
         */
        std::string _type;

        /*
         * The general, protocol neutral registration 
         * state of the account
         */
        RegistrationState _registrationState;
        
        /*
         * Details about the registration state.
         * This is a protocol Code:Description pair. 
         */
        std::pair<int, std::string> _registrationStateDetailed;

};

#endif
