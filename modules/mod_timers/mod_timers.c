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
#include <time.h>

#include "bgddl.h"

#ifdef TARGET_MAC
#include <SDL/SDL.h>
#else
#include <SDL.h>
#endif

#include "dlvaracc.h"

/* ----------------------------------------------------------------- */

#define    TIMER   0

/* ----------------------------------------------------------------- */
/* Definicion de variables globales (usada en tiempo de compilacion) */

char * __bgdexport( mod_timers, globals_def ) = "timer[9];\n";

/* ----------------------------------------------------------------- */
/* Son las variables que se desea acceder.                           */
/* El interprete completa esta estructura, si la variable existe.    */
/* (usada en tiempo de ejecucion)                                    */

DLVARFIXUP __bgdexport( mod_timers, globals_fixup )[] =
{
    /* Nombre de variable global, puntero al dato, tamaño del elemento, cantidad de elementos */
    { "timer"   , NULL, -1, -1 },
    { NULL, NULL, -1, -1 }
};

/* ----------------------------------------------------------------- */
/*
 *  FUNCTION : _advance_timers
 *
 *  Update the value of all global timers
 *
 *  PARAMS :
 *      None
 *
 *  RETURN VALUE :
 *      None
 */

static void _advance_timers( void )
{
    int * timer, i ;
    int curr_ticktimer = SDL_GetTicks() ;
    static int initial_ticktimer[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0} ;
    static int ltimer[10] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1} ; // -1 to force initial_ticktimer update

    /* TODO: Here add checking for console_mode, don't advance in this mode */
    timer = ( int * ) ( &GLODWORD( mod_timers, TIMER ) );
    for ( i = 0 ; i < 10 ; i++ )
    {
        if ( timer[i] != ltimer[i] ) initial_ticktimer[i] = curr_ticktimer - ( timer[i] * 10 ) ;
        ltimer[i] = timer[i] = ( curr_ticktimer - initial_ticktimer[i] ) / 10 ;
    }
}

/* ----------------------------------------------------------------- */

/* Bigest priority first execute
   Lowest priority last execute */

HOOK __bgdexport( mod_timers, handler_hooks )[] =
{
    { 100, _advance_timers },
    {   0, NULL            }
} ;

