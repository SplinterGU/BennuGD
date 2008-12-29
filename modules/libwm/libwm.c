/*
 *  Copyright © 2006-2008 SplinterGU (Fenix/Bennugd)
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

/* --------------------------------------------------------------------------- */

#ifdef TARGET_MAC
#include <SDL/SDL.h>
#else
#include <SDL.h>
#endif

#include "bgdrtm.h"

#include "bgddl.h"
#include "dlvaracc.h"

/* --------------------------------------------------------------------------- */

#define EXIT_STATUS         0
#define WINDOW_STATUS       1
#define FOCUS_STATUS        2
#define MOUSE_STATUS        3

/* --------------------------------------------------------------------------- */
/* Definicion de variables globales (usada en tiempo de compilacion) */

char * __bgdexport( libwm, globals_def ) =
    "exit_status = 0;\n"                /* SDL_QUIT status */
    "window_status = 1;\n"              /* MINIMIZED:0 VISIBLE:1 */
    "focus_status = 1;\n"               /* FOCUS status */
    "mouse_status = 1;\n";              /* MOUSE status (INSIDE WINDOW:1) */

/* --------------------------------------------------------------------------- */
/* Son las variables que se desea acceder.                           */
/* El interprete completa esta estructura, si la variable existe.    */
/* (usada en tiempo de ejecucion)                                    */

DLVARFIXUP  __bgdexport( libwm, globals_fixup )[] =
{
    /* Nombre de variable global, puntero al dato, tamaño del elemento, cantidad de elementos */
    { "exit_status"     , NULL, -1, -1 },
    { "window_status"   , NULL, -1, -1 },
    { "focus_status"    , NULL, -1, -1 },
    { "mouse_status"    , NULL, -1, -1 },
    { NULL              , NULL, -1, -1 }
};

/* --------------------------------------------------------------------------- */
/* Gestión de eventos de ventana                                               */
/* --------------------------------------------------------------------------- */

/*
 *  FUNCTION : wm_events
 *
 *  Process all pending wm SDL events, updating all global variables
 *
 *  PARAMS :
 *      None
 *
 *  RETURN VALUE :
 *      None
 */

static void wm_events()
{
    SDL_Event e ;

    /* Procesa los eventos de ventana pendientes */

    GLODWORD( libwm, EXIT_STATUS ) = 0 ;

    while ( SDL_PeepEvents( &e, 1, SDL_GETEVENT, SDL_QUITMASK | SDL_ACTIVEEVENTMASK ) )
    {
        switch ( e.type )
        {
                /* WINDOW MANAGER EVENTS */
            case SDL_QUIT:
                /* UPDATE  exit status... initilized each frame */
                GLODWORD( libwm, EXIT_STATUS ) = 1 ;
//                bgdrtm_exit( -1 );
                break ;

            case SDL_ACTIVEEVENT:
                if ( e.active.state & SDL_APPACTIVE ) GLODWORD( libwm, WINDOW_STATUS ) = ( e.active.gain ) ? 1 : 0 ;
                if ( e.active.state & SDL_APPINPUTFOCUS ) GLODWORD( libwm, FOCUS_STATUS ) = ( e.active.gain ) ? 1 : 0 ;
                if ( e.active.state & SDL_APPMOUSEFOCUS ) GLODWORD( libwm, MOUSE_STATUS ) = ( e.active.gain ) ? 1 : 0 ;
                break ;
        }
    }
}

/* --------------------------------------------------------------------------- */

/* Bigest priority first execute
   Lowest priority last execute */

HOOK __bgdexport( libwm, handler_hooks )[] =
{
    { 4700, wm_events   },
    {    0, NULL        }
} ;

/* --------------------------------------------------------------------------- */

char * __bgdexport( libwm, modules_dependency )[] =
{
    "libsdlhandler",
    NULL
};

/* --------------------------------------------------------------------------- */
