/*
 *  Copyright (C) 2004-2008 Savoir-Faire Linux inc.
 *  Author: Emmanuel Milou <emmanuel.milou@savoirfairelinux.com>
 *  Author: Yan Morin <yan.morin@savoirfairelinux.com>
 *  Author: Laurielle Lea <laurielle.lea@savoirfairelinux.com>
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

#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <assert.h>
#include <stdio.h>
#include <libintl.h>
#include <locale.h>

typedef float float32;
typedef short int16;


#ifdef DATAFORMAT_IS_FLOAT
#define SFLDataFormat float32
#define SFLDataFormatString "Float32"
#define SFLDataAmplitude 0.05
#define SFLConvertInt16(s) ((float)(s)-16384.0)/16384.0
#else
#define SFLDataFormat int16
#define SFLDataFormatString "Int16"
#define SFLDataAmplitude (32767 >> 4)
#define SFLConvertInt16(s) (s)
#endif

#ifdef SFLDEBUG
  #define _debug(...)          fprintf(stderr, "[sfl-debug] " __VA_ARGS__)
  #define _debugStart(...)     fprintf(stderr, "[sfl-debug] " __VA_ARGS__)
  #define _debugMid(...)       fprintf(stderr, __VA_ARGS__)
  #define _debugEnd(...)       fprintf(stderr, __VA_ARGS__)
  #define _debugException(...) fprintf(stderr, "[sfl-excep] " __VA_ARGS__ "\n")
  #define _debugInit(...)      fprintf(stderr, "[sfl-init] " __VA_ARGS__ "\n")
  #define _debugAlsa(...)      fprintf(stderr, "[alsa-debug] " __VA_ARGS__ )
#else
  #define _debug(...)
  #define _debugStart(...)
  #define _debugMid(...)
  #define _debugEnd(...)
  #define _debugException(...)
  #define _debugInit(...)
  #define _debugAlsa(...)
#endif

#define SFLPHONED_VERSION "0.8.2"
#define SFLPHONED_VERSIONNUM 0x000802

#define PROGNAME         "sflphoned"
#define PROGNAME_GLOBAL  "sflphone"
#define PROGDIR          "sflphone"
#define RINGDIR          "ringtones"
#define CODECDIR         "codecs"

#define _(arg) arg
#define MONO					1
#define CHANNELS				2
#define SIZEBUF 				1024*1024

#define ALSA_DFT_CARD_ID     0

#define PCM_HW		"hw"
#define PCM_PLUGHW	"plughw"
#define PCM_PULSE	"pulse"
#define PCM_FRONT	"plug:front"
#define PCM_DEFAULT	"default"
#define PCM_DMIX	"plug:dmix"
#define PCM_SURROUND40	"plug:surround40"
#define PCM_SURROUND41	"plug:surround41"
#define PCM_SURROUND50	"plug:surround50"
#define PCM_SURROUND51	"plug:surround51"
#define PCM_SURROUND71	"plug:surround71"
#define PCM_IEC958	"plug:iec958"

#define SFL_CODEC_VALID_PREFIX	"libcodec_"
#define SFL_CODEC_VALID_EXTEN	".so"
#define CURRENT_DIR		"."
#define PARENT_DIR		".."

#define SFL_PCM_BOTH		0x0021
#define SFL_PCM_PLAYBACK	0x0022
#define SFL_PCM_CAPTURE		0x0023

#ifdef USE_IAX
#define	IAX2_ENABLED  true
#else
#define	IAX2_ENABLED  false
#endif

#define GSM_STRING_DESCRIPTION	  "gsm"
#define SPEEX_STRING_DESCRIPTION  "speex"
#define ILBC_STRING_DESCRIPTION  "ilbc"
#define RINGTONE_ENABLED	  1
#define DISPLAY_DIALPAD		  1
#define START_HIDDEN		  1
#define WINDOW_POPUP		  1

// Error codes for error handling
#define NO_ERROR		      0x0000
#define ALSA_CAPTURE_DEVICE           0x0001
#define ALSA_PLAYBACK_DEVICE          0x0010
#define REGISTRATION_FORBIDDEN        0x0011
#define REGISTRATION_UNAUTHORIZED     0x0100
#define REGISTRATION_UNREACHABLE      0x0101
#define NETWORK_UNREACHABLE           0x0110

#endif	// __GLOBAL_H__
