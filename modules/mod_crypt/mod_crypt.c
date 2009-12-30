/*
 *  Copyright © 2006-2009 SplinterGU (Fenix/Bennugd)
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

#include "b_crypt.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bgddl.h"

/* ----------------------------------------------------------------- */

DLCONSTANT  __bgdexport( mod_crypt, constants_def)[] =
{
    { "CRYPT_NONE"  , TYPE_INT  , CRYPT_NONE    },
    { "CRYPT_DES"   , TYPE_INT  , CRYPT_DES     },
    { "CRYPT_3DES"  , TYPE_INT  , CRYPT_3DES    },

    { NULL          , 0         , 0             }
} ;

/* ----------------------------------------------------------------- */

static int modcrypt_new( INSTANCE * my, int * params )
{
    return ( ( int ) crypt_create( params[0], ( char * ) params[1] ) );
}

/* ----------------------------------------------------------------- */

static int modcrypt_del( INSTANCE * my, int * params )
{
    crypt_destroy( ( crypt_handle * ) params[0] );
    return 1;
}

/* ----------------------------------------------------------------- */

static int __crypt( crypt_handle * ch, char * in, char * out, int blocks, int enc )
{
    if ( !ch || !in || !out || blocks <= 0 ) return -1;

    while ( blocks-- )
    {
        if ( crypt_data( ch, in, out, 8, enc ) <= 0 ) return -1;
        in += 8;
        out += 8;
    }

    return 0;
}

/* ----------------------------------------------------------------- */

static int modcrypt_encrypt( INSTANCE * my, int * params )
{
    return ( __crypt( ( crypt_handle * ) params[0], ( char * ) params[1], ( char * ) params[2], params[3], 1 ) );
}

/* ----------------------------------------------------------------- */

static int modcrypt_decrypt( INSTANCE * my, int * params )
{
    return ( __crypt( ( crypt_handle * ) params[0], ( char * ) params[1], ( char * ) params[2], params[3], 0 ) );
}

/* ----------------------------------------------------------------- */

static int modcrypt_encrypt2( INSTANCE * my, int * params )
{
    int r;
    crypt_handle * ch = crypt_create( params[0], ( char * ) params[1] );
    r = __crypt( ch, ( char * ) params[2], ( char * ) params[3], params[4], 1 );
    crypt_destroy( ch );
    return r;
}

/* ----------------------------------------------------------------- */

static int modcrypt_decrypt2( INSTANCE * my, int * params )
{
    int r;
    crypt_handle * ch = crypt_create( params[0], ( char * ) params[1] );
    r = __crypt( ch, ( char * ) params[2], ( char * ) params[3], params[4], 0 );
    crypt_destroy( ch );
    return r;
}

/* ----------------------------------------------------------------- */
/* Functions declaration                                             */

DLSYSFUNCS  __bgdexport( mod_crypt, functions_exports)[] =
{
    { "CRYPT_NEW"       , "IP"      , TYPE_POINTER     , modcrypt_new       },
    { "CRYPT_DEL"       , "P"       , TYPE_INT         , modcrypt_del       },
    { "CRYPT_ENCRYPT"   , "PPPI"    , TYPE_INT         , modcrypt_encrypt   },
    { "CRYPT_DECRYPT"   , "PPPI"    , TYPE_INT         , modcrypt_decrypt   },
    { "CRYPT_ENCRYPT"   , "IPPPI"   , TYPE_INT         , modcrypt_encrypt2  },
    { "CRYPT_DECRYPT"   , "IPPPI"   , TYPE_INT         , modcrypt_decrypt2  },

    { 0                 , 0         , 0                , 0                  }
};

/* ----------------------------------------------------------------- */
