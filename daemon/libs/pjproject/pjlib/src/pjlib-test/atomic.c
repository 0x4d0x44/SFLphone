/* $Id: atomic.c 2394 2008-12-23 17:27:53Z bennylp $ */
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
#include "test.h"
#include <pjlib.h>

/**
 * \page page_pjlib_atomic_test Test: Atomic Variable
 *
 * This file provides implementation of \b atomic_test(). It tests the
 * functionality of the atomic variable API.
 *
 * \section atomic_test_sec Scope of the Test
 *
 * API tested:
 *  - pj_atomic_create()
 *  - pj_atomic_get()
 *  - pj_atomic_inc()
 *  - pj_atomic_dec()
 *  - pj_atomic_set()
 *  - pj_atomic_destroy()
 *
 *
 * This file is <b>pjlib-test/atomic.c</b>
 *
 * \include pjlib-test/atomic.c
 */


#if INCLUDE_ATOMIC_TEST

int atomic_test(void)
{
    pj_pool_t *pool;
    pj_atomic_t *atomic_var;
    pj_status_t rc;

    pool = pj_pool_create(mem, NULL, 4096, 0, NULL);
    if (!pool)
        return -10;

    /* create() */
    rc = pj_atomic_create(pool, 111, &atomic_var);
    if (rc != 0) {
        return -20;
    }

    /* get: check the value. */
    if (pj_atomic_get(atomic_var) != 111)
        return -30;

    /* increment. */
    pj_atomic_inc(atomic_var);
    if (pj_atomic_get(atomic_var) != 112)
        return -40;

    /* decrement. */
    pj_atomic_dec(atomic_var);
    if (pj_atomic_get(atomic_var) != 111)
        return -50;

    /* set */
    pj_atomic_set(atomic_var, 211);
    if (pj_atomic_get(atomic_var) != 211)
        return -60;

    /* add */
    pj_atomic_add(atomic_var, 10);
    if (pj_atomic_get(atomic_var) != 221)
        return -60;

    /* check the value again. */
    if (pj_atomic_get(atomic_var) != 221)
        return -70;

    /* destroy */
    rc = pj_atomic_destroy(atomic_var);
    if (rc != 0)
        return -80;

    pj_pool_release(pool);

    return 0;
}


#else
/* To prevent warning about "translation unit is empty"
 * when this test is disabled. 
 */
int dummy_atomic_test;
#endif  /* INCLUDE_ATOMIC_TEST */

