/*
 *  Copyright � 2006-2019 SplinterGU (Fenix/Bennugd)
 *  Copyright � 2002-2006 Fenix Team (Fenix)
 *  Copyright � 1999-2002 Jos� Luis Cebri�n Pag�e (Fenix)
 *
 *  This file is part of Bennu - Game Development
 *
 *  This software is provided 'as-is', without any express or implied
 *  warranty. In no event will the authors be held liable for any damages
 *  arising from the use of this software.
 *
 *  Permission is granted to anyone to use this software for any purpose,
 *  including commercial applications, and to alter it and redistribute it
 *  freely, subject to the following restrictions:
 *
 *     1. The origin of this software must not be misrepresented; you must not
 *     claim that you wrote the original software. If you use this software
 *     in a product, an acknowledgment in the product documentation would be
 *     appreciated but is not required.
 *
 *     2. Altered source versions must be plainly marked as such, and must not be
 *     misrepresented as being the original software.
 *
 *     3. This notice may not be removed or altered from any source
 *     distribution.
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

/* --------------------------------------------------------------------------- */

#ifdef __GNUC__
#ifndef _inline
#define _inline inline
#endif
#endif

/* --------------------------------------------------------------------------- */
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

/* --------------------------------------------------------------------------- */

static int modkey_key( INSTANCE * my, int * params )
{
    return ( _get_key( params[0] ) );
}

/* --------------------------------------------------------------------------- */
/* exports                                                                     */
/* --------------------------------------------------------------------------- */

#include "mod_key_exports.h"

/* --------------------------------------------------------------------------- */
