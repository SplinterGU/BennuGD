/*
 *  Copyright © 2006-2009 SplinterGU (Fenix/Bennugd)
 *  Copyright © 2002-2006 Fenix Team (Fenix)
 *  Copyright © 1999-2002 José Luis Cebrián Pagüe (Fenix)
 *
 *  This file is part of Bennu - Game Development
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "bgddl.h"
#include "xctype.h"
#include "libkey.h"

/* ----------------------------------------------------------------- */

#ifdef __GNUC__
#define _inline inline
#endif

/* ----------------------------------------------------------------- */
/*
 *  FUNCTION : _get_key
 *
 *  Returns the current status of a key (pressed or not)
 *
 *  PARAMS :
 *      key             Internal code of the key
 *
 *  RETURN VALUE :
 *      A non-zero positive value if the key is pressed, 0 otherwise
 */

static _inline int _get_key( int code )
{
    key_equiv * curr ;
    int found = 0 ;

    if ( !keystate ) return 0;

    curr = &key_table[code] ;

    while ( curr && found == 0 )
    {
        found = keystate[curr->sdlk_equiv] ;
        curr = curr->next ;
    }

    return found ;
}

/* ----------------------------------------------------------------- */

static int modkey_key( INSTANCE * my, int * params )
{
    return ( _get_key( params[0] ) );
}

/* ----------------------------------------------------------------- */

DLSYSFUNCS  __bgdexport( mod_key, functions_exports )[] =
{
    { "KEY" , "I"   , TYPE_INT  , modkey_key   },
    { 0     , 0     , 0         , 0            }
};

/* ----------------------------------------------------------------- */

char * __bgdexport( mod_key, modules_dependency )[] =
{
    "libkey",
    NULL
};

/* ----------------------------------------------------------------- */
