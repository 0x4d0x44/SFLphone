/* $Id: cc_armcc.h 3046 2010-01-06 08:34:41Z bennylp $ */
/* 
 * Copyright (C) 2008-2009 Teluu Inc. (http://www.teluu.com)
 * Copyright (C) 2003-2008 Benny Prijono <benny@prijono.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 *
 *  Additional permission under GNU GPL version 3 section 7:
 *
 *  If you modify this program, or any covered work, by linking or
 *  combining it with the OpenSSL project's OpenSSL library (or a
 *  modified version of that library), containing parts covered by the
 *  terms of the OpenSSL or SSLeay licenses, Teluu Inc. (http://www.teluu.com)
 *  grants you additional permission to convey the resulting work.
 *  Corresponding Source for a non-source form of such a combination
 *  shall include the source code for the parts of OpenSSL used as well
 *  as that of the covered work.
 */
#ifndef __PJ_COMPAT_CC_ARMCC_H__
#define __PJ_COMPAT_CC_ARMCC_H__

/**
 * @file cc_armcc.h
 * @brief Describes ARMCC compiler specifics.
 */

#ifndef __ARMCC__
#  error "This file is only for armcc!"
#endif

#define PJ_CC_NAME		"armcc"
#define PJ_CC_VER_1             (__ARMCC_VERSION/100000)
#define PJ_CC_VER_2             ((__ARMCC_VERSION%100000)/10000)
#define PJ_CC_VER_3             (__ARMCC_VERSION%10000)

#ifdef __cplusplus
#  define PJ_INLINE_SPECIFIER	inline
#else
#  define PJ_INLINE_SPECIFIER	static __inline
#endif

#define PJ_THREAD_FUNC	
#define PJ_NORETURN		
#define PJ_ATTR_NORETURN	__attribute__ ((noreturn))

#define PJ_HAS_INT64		1

typedef long long pj_int64_t;
typedef unsigned long long pj_uint64_t;

#define PJ_INT64_FMT		"L"

#define PJ_UNREACHED(x)	    	

#endif	/* __PJ_COMPAT_CC_ARMCC_H__ */
