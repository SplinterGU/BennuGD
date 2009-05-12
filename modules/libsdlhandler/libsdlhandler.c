/*
 *  Copyright © 2006-2009 SplinterGU (Fenix/Bennugd)
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

#include "bgddl.h"

#ifdef TARGET_MAC
#include <SDL/SDL.h>
#else
#include <SDL.h>
#endif

/* ----------------------------------------------------------------- */
/* Public functions                                                  */

static void  dump_new_events()
{
    SDL_Event event;
    /* Remove all pendings events */

    /* We can't return -1, just return 0 (no event) on error */
    while ( SDL_PeepEvents( &event, 1, SDL_GETEVENT, SDL_ALLEVENTS ) > 0 );

    /* Get new events */
    SDL_PumpEvents();
}

/* ----------------------------------------------------------------- */
/* Funciones de inicializacion del modulo/plugin                     */

void __bgdexport( libsdlhandler, module_initialize )()
{
    if ( !SDL_WasInit( SDL_INIT_EVENTTHREAD ) ) SDL_InitSubSystem( SDL_INIT_EVENTTHREAD );
}

/* ----------------------------------------------------------------- */

void __bgdexport( libsdlhandler, module_finalize )()
{
    if ( SDL_WasInit( SDL_INIT_EVENTTHREAD ) ) SDL_QuitSubSystem( SDL_INIT_EVENTTHREAD );
}

/* ----------------------------------------------------------------- */

/* Bigest priority first execute
   Lowest priority last execute */

HOOK __bgdexport( libsdlhandler, handler_hooks )[] =
{
    { 5000, dump_new_events                   },
    {    0, NULL                              }
} ;

/* ----------------------------------------------------------------- */
