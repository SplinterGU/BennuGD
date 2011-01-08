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

/* ------------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "b_crypt.h"

/* ------------------------------------------------------------------------- */

crypt_handle * crypt_create( int method, char * key )
{
    crypt_handle * ch = calloc( sizeof( crypt_handle ), 1 );
    if ( !ch ) return NULL;

    ch->method = method;

    switch ( method )
    {
        case    CRYPT_DES  :
                if ( !DES_key_sched( ( DES_cblock * ) key, &ch->ks[KEY0] ) ) return ch;
                break;

        case    CRYPT_3DES :
                if (  DES_key_sched( ( DES_cblock * )          key, &ch->ks[KEY0] ) ) { free( ch ); return NULL; }
                if (  DES_key_sched( ( DES_cblock * ) ( 8  + key ), &ch->ks[KEY1] ) ) { free( ch ); return NULL; }
                if ( !DES_key_sched( ( DES_cblock * ) ( 16 + key ), &ch->ks[KEY2] ) ) return ch;
                break;
    }

    free( ch );
    return NULL;
}

/* ------------------------------------------------------------------------- */

void crypt_destroy( crypt_handle * ch )
{
    if ( ch ) free( ch );
}

/* ------------------------------------------------------------------------- */

int crypt_data( crypt_handle * ch, char * in, char * out, int size, int enc )
{
    DES_cblock aux;

    if ( !ch || ( size < 1 || size > 8 ) ) return -1;

    if ( enc && size < 8 ) memset( &in[size], '\0', 8 - size );

    switch( ch->method )
    {
        case    CRYPT_DES   :
                if ( enc )
                    DES_ecb_encrypt( ( DES_cblock * ) in, ( DES_cblock * ) out, &ch->ks[KEY0], DES_ENCRYPT );
                else
                    DES_ecb_encrypt( ( DES_cblock * ) in, ( DES_cblock * ) out, &ch->ks[KEY0], DES_DECRYPT );
                break;

        case    CRYPT_3DES  :
                if ( enc )
                {
                    DES_ecb_encrypt( ( DES_cblock * )   in, ( DES_cblock * )  out, &ch->ks[KEY0], DES_ENCRYPT );
                    DES_ecb_encrypt( ( DES_cblock * )  out, ( DES_cblock * ) &aux, &ch->ks[KEY1], DES_DECRYPT );
                    DES_ecb_encrypt( ( DES_cblock * ) &aux, ( DES_cblock * )  out, &ch->ks[KEY2], DES_ENCRYPT );
                }
                else
                {
                    DES_ecb_encrypt( ( DES_cblock * )   in, ( DES_cblock * )  out, &ch->ks[KEY2], DES_DECRYPT );
                    DES_ecb_encrypt( ( DES_cblock * )  out, ( DES_cblock * ) &aux, &ch->ks[KEY1], DES_ENCRYPT );
                    DES_ecb_encrypt( ( DES_cblock * ) &aux, ( DES_cblock * )  out, &ch->ks[KEY0], DES_DECRYPT );
                }
                break;
    }

    return ( enc ? 8 : size );
}

/* ------------------------------------------------------------------------- */
