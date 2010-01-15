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

/* --------------------------------------------------------------------------- */

#ifdef TARGET_MAC
#include <SDL/SDL.h>
#else
#include <SDL.h>
#endif

#include "bgddl.h"
#include "dlvaracc.h"

#include "libgrbase.h"
#include "libvideo.h"
#include "libblit.h"
#include "librender.h"

#include "sysprocs_st.h"

/* --------------------------------------------------------------------------- */

static int last_mousex = 0;
static int last_mousey = 0;
static int last_mousez = 0;
static int last_mousefile = 0;
static int last_mousegraph = 0;
static int last_mouseangle = 0;
static int last_mousesize = 0;
static int last_mouseflags = 0;
static int last_mouseregion = 0;
static int last_mousecenterx = -1;
static int last_mousecentery = -1;

static GRAPH * mouse_map = NULL;

/* --------------------------------------------------------------------------- */

#define MOUSEX              0
#define MOUSEY              1
#define MOUSEZ              2
#define MOUSEFILE           3
#define MOUSEGRAPH          4
#define MOUSEANGLE          5
#define MOUSESIZE           6
#define MOUSEFLAGS          7
#define MOUSEREGION         8
#define MOUSELEFT           9
#define MOUSEMIDDLE         10
#define MOUSERIGHT          11
#define MOUSEWHEELUP        12
#define MOUSEWHEELDOWN      13

/* --------------------------------------------------------------------------- */

char * __bgdexport( libmouse, globals_def ) =
    "STRUCT mouse\n"
    "x = 99999, y = 99999;\n"
    "z = -512;\n"
    "file;\n"
    "graph;\n"
    "angle;\n"
    "size = 100;\n"
    "flags;\n"
    "region;\n"
    "left, middle, right;\n"
    "wheelup, wheeldown;\n"
    "END\n";

/* --------------------------------------------------------------------------- */

DLVARFIXUP __bgdexport( libmouse, globals_fixup )[] =
{
    /* Nombre de variable global, puntero al dato, tamaño del elemento, cantidad de elementos */

    { "mouse.x"         , NULL, -1, -1 },
    { "mouse.y"         , NULL, -1, -1 },
    { "mouse.z"         , NULL, -1, -1 },
    { "mouse.file"      , NULL, -1, -1 },
    { "mouse.graph"     , NULL, -1, -1 },
    { "mouse.angle"     , NULL, -1, -1 },
    { "mouse.size"      , NULL, -1, -1 },
    { "mouse.flags"     , NULL, -1, -1 },
    { "mouse.region"    , NULL, -1, -1 },
    { "mouse.left"      , NULL, -1, -1 },
    { "mouse.middle"    , NULL, -1, -1 },
    { "mouse.right"     , NULL, -1, -1 },
    { "mouse.wheelup"   , NULL, -1, -1 },
    { "mouse.wheeldown" , NULL, -1, -1 },

    { NULL              , NULL, -1, -1 }
};

/* --------------------------------------------------------------------------- */

/*
 *  FUNCTION : mouse_events
 *
 *  Process all pending SDL events, updating all global variables
 *  and handling debug key presses
 *  and cannot be changed
 *
 *  PARAMS :
 *      None
 *
 *  RETURN VALUE :
 *      None
 */

static void do_mouse_events()
{
    SDL_Event e ;
    static int last_mouse_x = -1, last_mouse_y = -1 ;

    /* Actualizar eventos */

    /* El cambio de mouse.x/y afecta directamente al ratón */

    if (
        ( last_mouse_x != -1 && GLOINT32( libmouse, MOUSEX ) != last_mouse_x ) ||
        ( last_mouse_y != -1 && GLOINT32( libmouse, MOUSEY ) != last_mouse_y ) )
    {
        if ( scale_resolution != 0 )
        {
            SDL_WarpMouse( GLOINT32( libmouse, MOUSEX ) / ( (double)screen->w / (double)scale_screen->w ), GLOINT32( libmouse, MOUSEY ) / ( (double)screen->h / (double)scale_screen->h ) ) ;
        }
        else if ( enable_scale || scale_mode != SCALE_NONE )
        {
            SDL_WarpMouse( GLOINT32( libmouse, MOUSEX ) * 2 , GLOINT32( libmouse, MOUSEY ) * 2 ) ;
        }
        else
        {
            SDL_WarpMouse( GLOINT32( libmouse, MOUSEX ), GLOINT32( libmouse, MOUSEY ) ) ;
        }
    }

    /* Procesa los eventos de mouse pendientes */

    GLODWORD( libmouse, MOUSEWHEELUP )   = 0 ;
    GLODWORD( libmouse, MOUSEWHEELDOWN ) = 0 ;

    while ( SDL_PeepEvents( &e, 1, SDL_GETEVENT, SDL_MOUSEEVENTMASK ) > 0 )
    {
        switch ( e.type )
        {
            case SDL_MOUSEMOTION:
                if ( scale_resolution != 0 )
                {
                    GLOINT32( libmouse, MOUSEX ) = e.motion.x * ( (double)screen->w / (double)scale_screen->w );
                    GLOINT32( libmouse, MOUSEY ) = e.motion.y * ( (double)screen->h / (double)scale_screen->h );
                }
                else if ( enable_scale || scale_mode != SCALE_NONE )
                {
                    GLOINT32( libmouse, MOUSEX ) = e.motion.x / 2 ;
                    GLOINT32( libmouse, MOUSEY ) = e.motion.y / 2 ;
                }
                else
                {
                    GLOINT32( libmouse, MOUSEX ) = e.motion.x ;
                    GLOINT32( libmouse, MOUSEY ) = e.motion.y ;
                }
                break ;

            case SDL_MOUSEBUTTONDOWN:
                if ( e.button.button == SDL_BUTTON_LEFT )      GLODWORD( libmouse, MOUSELEFT )     = 1 ;
                if ( e.button.button == SDL_BUTTON_MIDDLE )    GLODWORD( libmouse, MOUSEMIDDLE )   = 1 ;
                if ( e.button.button == SDL_BUTTON_RIGHT )     GLODWORD( libmouse, MOUSERIGHT )    = 1 ;
                if ( e.button.button == SDL_BUTTON_WHEELUP )   GLODWORD( libmouse, MOUSEWHEELUP )++ ;
                if ( e.button.button == SDL_BUTTON_WHEELDOWN ) GLODWORD( libmouse, MOUSEWHEELDOWN )++ ;
                break ;

            case SDL_MOUSEBUTTONUP:
                if ( e.button.button == SDL_BUTTON_LEFT )      GLODWORD( libmouse, MOUSELEFT )      = 0 ;
                if ( e.button.button == SDL_BUTTON_MIDDLE )    GLODWORD( libmouse, MOUSEMIDDLE )    = 0 ;
                if ( e.button.button == SDL_BUTTON_RIGHT )     GLODWORD( libmouse, MOUSERIGHT )     = 0 ;
                break ;
        }
    }

    last_mouse_x = GLOINT32( libmouse, MOUSEX ) ;
    last_mouse_y = GLOINT32( libmouse, MOUSEY ) ;
}

/* --------------------------------------------------------------------------- */

/*
 *  FUNCTION : mouse_info
 *
 *  Returns information about the mouse
 *
 *  PARAMS :
 *      ptr             Void pointer, used for compatibility with DLL-type objects
 *      clip            Region to fill with bounding box
 *
 *  RETURN VALUE :
 *      1 if the mouse has changed since last call
 */

static int mouse_info( INSTANCE * i, REGION * clip, int * z, int * drawme )
{
    int mousex ;
    int mousey ;
    int mousez ;
    int mousefile ;
    int mousegraph ;
    int mouseangle;
    int mousesize;
    int mouseflags;
    int mouseregion;
    int changed;

    * z = mousez = GLOINT32( libmouse, MOUSEZ );

    mousefile   = GLODWORD( libmouse, MOUSEFILE );
    mousegraph  = GLODWORD( libmouse, MOUSEGRAPH );

    mouse_map = ( mousegraph > 0 ) ? bitmap_get( mousefile, mousegraph ) : NULL ;
    if ( !mouse_map )
    {
        * drawme = 0;
        /*
        clip->x = clip->x2 = -2;
        clip->y = clip->y2 = -2;
        */
        return 0;
    }

    mousex      = GLOINT32( libmouse, MOUSEX );
    mousey      = GLOINT32( libmouse, MOUSEY );
//    mousez      = GLODWORD( libmouse, MOUSEZ );
    mouseangle  = GLOINT32( libmouse, MOUSEANGLE );
    mousesize   = GLOINT32( libmouse, MOUSESIZE );
    mouseflags  = GLODWORD( libmouse, MOUSEFLAGS );
    mouseregion = GLOINT32( libmouse, MOUSEREGION );

    * drawme = 1;

    changed =
        ( mouse_map && mouse_map->modified ) ||
        last_mousex      != mousex      ||
        last_mousey      != mousey      ||
        last_mousez      != mousez      ||
        last_mousefile   != mousefile   ||
        last_mousegraph  != mousegraph  ||
        last_mouseangle  != mouseangle  ||
        last_mousesize   != mousesize   ||
        last_mouseflags  != mouseflags  ||
        last_mouseregion != mouseregion ||
        (
            mouse_map->ncpoints &&
            (
                last_mousecenterx != mouse_map->cpoints[0].x ||
                last_mousecentery != mouse_map->cpoints[0].y
            )
        )
        ;

    if ( changed )
    {
        last_mousex         = mousex ;
        last_mousey         = mousey ;
        last_mousez         = mousez ;
        last_mousefile      = mousefile ;
        last_mousegraph     = mousegraph ;
        last_mouseangle     = mouseangle ;
        last_mousesize      = mousesize ;
        last_mouseflags     = mouseflags ;
        last_mouseregion    = mouseregion ;
        if ( mouse_map->ncpoints )
        {
            last_mousecenterx = mouse_map->cpoints[0].x;
            last_mousecentery = mouse_map->cpoints[0].y;
        }
        else
        {
            last_mousecenterx = CPOINT_UNDEFINED;
            last_mousecentery = CPOINT_UNDEFINED;
        }

        gr_get_bbox(
            clip,
            0,
            mousex,
            mousey,
            mouseflags,
            mouseangle ,
            mousesize ,
            mousesize ,
            mouse_map ) ;
    }

    return changed;
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : mouse_draw
 *
 *  Draws the mouse graphic at screen
 *
 *  PARAMS :
 *      ptr             Void pointer, used for compatibility with DLL-type objects
 *      clip            Clipping region
 *
 *  RETURN VALUE :
 *      None
 */

static void mouse_draw( INSTANCE * i, REGION * clip )
{
    int r ;
    REGION region;

    r = GLOINT32( libmouse, MOUSEREGION ) ;
    if ( r < 0 || r > 31 ) r = 0 ;

    region = regions[r];
    if ( clip ) region_union( &region, clip );

    if ( GLOINT32( libmouse, MOUSEANGLE ) || GLOINT32( libmouse, MOUSESIZE ) != 100 )
        gr_rotated_blit(
            0,
            &region,
            GLOINT32( libmouse, MOUSEX ),
            GLOINT32( libmouse, MOUSEY ),
            GLODWORD( libmouse, MOUSEFLAGS ),
            GLOINT32( libmouse, MOUSEANGLE ),
            GLOINT32( libmouse, MOUSESIZE ),
            GLOINT32( libmouse, MOUSESIZE ),
            mouse_map ) ;
    else
        gr_blit(
            0,
            &region,
            GLOINT32( libmouse, MOUSEX ),
            GLOINT32( libmouse, MOUSEY ),
            GLODWORD( libmouse, MOUSEFLAGS ),
            mouse_map ) ;

    mouse_map->modified = 0;
}

/* --------------------------------------------------------------------------- */

/* Bigest priority first execute
   Lowest priority last execute */

HOOK __bgdexport( libmouse, handler_hooks )[] =
{
    { 4800, do_mouse_events },
    { 0, NULL }
} ;

/* --------------------------------------------------------------------------- */

char * __bgdexport( libmouse, modules_dependency )[] =
{
    "libsdlhandler",
    "libgrbase",
    "libvideo",
    "libblit",
    "librender", // Add by Sandman
    NULL
};

/* --------------------------------------------------------------------------- */

void __bgdexport( libmouse, module_initialize )()
{
    gr_new_object( GLOINT32( libmouse, MOUSEZ ), mouse_info, mouse_draw, 0 );
}

/* --------------------------------------------------------------------------- */
