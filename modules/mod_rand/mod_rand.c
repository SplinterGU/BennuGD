/*
 *  Copyright © 2006-2011 SplinterGU (Fenix/Bennugd)
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

#include <stdlib.h>

#include "bgddl.h"
#include "fmath.h"

/* ---------------------------------------------------------------------- */

static int rand_seed( INSTANCE * my, int * params )
{
    srand( params[0] ) ;
    return 1 ;
}

/* ---------------------------------------------------------------------- */

static int rand_std( INSTANCE * my, int * params )
{
    int num1 = MIN( params[0], params[1] ) ;
    int num2 = MAX( params[0], params[1] ) ;
    int var = num2 - num1 + 1;

    if ( var > RAND_MAX )
        return num1 + rand() * ((( double ) var ) / RAND_MAX );
    else
        return num1 + rand() % var;
}

/* ---------------------------------------------------------------------- */
/* Declaracion de funciones                                               */

DLSYSFUNCS  __bgdexport( mod_rand, functions_exports )[] =
{
    { "RAND_SEED"   , "I"   , TYPE_INT  , rand_seed     },
    { "RAND"        , "II"  , TYPE_INT  , rand_std      },
    { 0             , 0     , 0         , 0             }
};

/* ---------------------------------------------------------------------- */
