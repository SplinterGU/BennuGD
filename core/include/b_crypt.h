/*
 *  Copyright © 2006-2011 SplinterGU (Fenix/Bennugd)
 *
 *  This crypt is part of Bennu - Game Development
 *
 *  Bennu is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  Bennu is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 */

#ifndef __B_CRYPT_H
#define __B_CRYPT_H

/* ------------------------------------------------------------------------- */

#include <openssl/des.h>

/* ------------------------------------------------------------------------- */

enum {
    CRYPT_NONE = 0,
    CRYPT_DES,
    CRYPT_3DES
};

enum {
    KEY0 = 0,
    KEY1,
    KEY2,
    MAX_KEYS
};

/* ------------------------------------------------------------------------- */

typedef struct
{
    int method;
    DES_key_schedule ks[MAX_KEYS];
} crypt_handle;

/* ------------------------------------------------------------------------- */

extern crypt_handle * crypt_create( int method, char * key );
extern void crypt_destroy( crypt_handle * ch );
extern int crypt_data( crypt_handle * ch, char * in, char * out, int size, int enc );

/* ------------------------------------------------------------------------- */

#endif
