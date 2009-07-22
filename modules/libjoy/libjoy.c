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
/* Thanks Sandman for suggest on openjoys at initialization time               */
/* --------------------------------------------------------------------------- */
/* Credits SplinterGU/Sandman 2007-2009                                        */
/* --------------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef TARGET_MAC
#include <SDL/SDL.h>
#else
#include <SDL.h>
#endif

/* --------------------------------------------------------------------------- */

#include "bgddl.h"

#include "bgdrtm.h"

#include "files.h"
#include "xstrings.h"

/* --------------------------------------------------------------------------- */

#define MAX_JOYS    32

static int _max_joys = 0;
static SDL_Joystick * _joysticks[MAX_JOYS];
static int _selected_joystick = -1;

/* --------------------------------------------------------------------------- */
/**
 * libjoy_num ()
 * Returns the number of joysticks present in the system
 **/

int libjoy_num( void )
{
    return _max_joys ;
}

/* --------------------------------------------------------------------------- */
/**
 * libjoy_name (int JOY)
 * Returns the name for a given joystick present in the system
 **/

int libjoy_name( int joy )
{
    int result;
    result = string_new( SDL_JoystickName( joy ) );
    string_use( result );
    return result;
}

/* --------------------------------------------------------------------------- */
/**
 * libjoy_select (int JOY)
 * Returns the selected joystick number
 **/

int libjoy_select( int joy )
{
    return ( _selected_joystick = joy );
}

/* --------------------------------------------------------------------------- */
/**
 * libjoy_buttons ()
 * Returns the selected joystick total buttons
 **/

int libjoy_buttons( void )
{
    if ( _selected_joystick >= 0 && _selected_joystick < _max_joys )
    {
        return SDL_JoystickNumButtons( _joysticks[_selected_joystick] ) ;
    }
    return -1 ;
}

/* --------------------------------------------------------------------------- */
/**
 * libjoy_axes ()
 * Returns the selected joystick total axes
 **/

int libjoy_axes( void )
{
    if ( _selected_joystick >= 0 && _selected_joystick < _max_joys )
    {
        return SDL_JoystickNumAxes( _joysticks[_selected_joystick] ) ;
    }
    return -1 ;
}

/* --------------------------------------------------------------------------- */
/**
 * libjoy_get_button ( int button )
 * Returns the selected joystick state for the given button
 **/

int libjoy_get_button( int button )
{
    if ( _selected_joystick >= 0 && _selected_joystick < _max_joys )
    {
        return SDL_JoystickGetButton( _joysticks[_selected_joystick], button ) ;
    }
    return -1 ;
}

/* --------------------------------------------------------------------------- */
/**
 * libjoy_get_position ( int axis )
 * Returns the selected joystick state for the given axis
 **/

int libjoy_get_position( int axis )
{
    if ( _selected_joystick >= 0 && _selected_joystick < _max_joys )
    {
        return SDL_JoystickGetAxis( _joysticks[_selected_joystick], axis ) ;
    }
    return -1 ;
}

/* --------------------------------------------------------------------------- */
/**
 * libjoy_hats ()
 * Returns the total number of POV hats of the current selected joystick
 **/

int libjoy_hats( void )
{
    if ( _selected_joystick >= 0 && _selected_joystick < _max_joys )
    {
        return SDL_JoystickNumHats( _joysticks[_selected_joystick] ) ;
    }
    return -1 ;
}

/* --------------------------------------------------------------------------- */
/**
 * libjoy_balls ()
 * Returns the total number of balls of the current selected joystick
 **/

int libjoy_balls( void )
{
    if ( _selected_joystick >= 0 && _selected_joystick < _max_joys )
    {
        return SDL_JoystickNumBalls( _joysticks[_selected_joystick] ) ;
    }
    return -1 ;
}

/* --------------------------------------------------------------------------- */
/**
 * libjoy_get_hat (int HAT)
 * Returns the state of the specfied hat on the current selected joystick
 **/

int libjoy_get_hat( int hat )
{
    if ( _selected_joystick >= 0 && _selected_joystick < _max_joys )
    {
        if ( hat >= 0 && hat <= SDL_JoystickNumHats( _joysticks[_selected_joystick] ) )
        {
            return SDL_JoystickGetHat( _joysticks[_selected_joystick], hat ) ;
        }
    }
    return -1 ;
}

/* --------------------------------------------------------------------------- */
/**
 * libjoy_get_ball (int BALL, int* dx, int* dy)
 * Returns the state of the specfied ball on the current selected joystick
 **/

int libjoy_get_ball( int ball, int * dx, int * dy )
{
    if ( _selected_joystick >= 0 && _selected_joystick < _max_joys )
    {
        if ( ball >= 0 && ball <= SDL_JoystickNumBalls( _joysticks[ball] ) )
        {
            return SDL_JoystickGetBall( _joysticks[_selected_joystick], ball, dx, dy ) ;
        }
    }
    return -1 ;
}

/* --------------------------------------------------------------------------- */
/* --------------------------------------------------------------------------- */
/* --------------------------------------------------------------------------- */

/* --------------------------------------------------------------------------- */
/**
 * libjoy_buttons_specific (int JOY)
 * Returns the selected joystick total buttons
 **/

int libjoy_buttons_specific( int joy )
{
    if ( joy >= 0 && joy < _max_joys )
    {
        return SDL_JoystickNumButtons( _joysticks[joy] ) ;
    }
    return -1 ;
}

/* --------------------------------------------------------------------------- */
/**
 * libjoy_axes_specific (int JOY)
 * Returns the selected joystick total axes
 **/

int libjoy_axes_specific( int joy )
{
    if ( joy >= 0 && joy < _max_joys )
    {
        return SDL_JoystickNumAxes( _joysticks[joy] ) ;
    }
    return -1 ;
}

/* --------------------------------------------------------------------------- */
/**
 * libjoy_get_button_specific (int JOY, int button)
 * Returns the selected joystick state for the given button
 **/

int libjoy_get_button_specific( int joy, int button )
{
    if ( joy >= 0 && joy < _max_joys )
    {
        if ( button >= 0 && button <= SDL_JoystickNumButtons( _joysticks[joy] ) )
        {
            return SDL_JoystickGetButton( _joysticks[joy], button ) ;
        }
    }
    return -1 ;
}

/* --------------------------------------------------------------------------- */
/**
 * libjoy_get_position_specific (int JOY, int axis)
 * Returns the selected joystick state for the given axis
 **/

int libjoy_get_position_specific( int joy, int axis )
{
    if ( joy >= 0 && joy < _max_joys )
    {
        if ( axis >= 0 && axis <= SDL_JoystickNumAxes( _joysticks[joy] ) )
        {
            return SDL_JoystickGetAxis( _joysticks[joy], axis ) ;
        }
    }
    return -1 ;
}

/* --------------------------------------------------------------------------- */
/* Added by Sandman */
/* --------------------------------------------------------------------------- */
/**
 * libjoy_hats_specific (int JOY)
 * Returns the total number of POV hats of the specified joystick
 **/

int libjoy_hats_specific( int joy )
{
    if ( joy >= 0 && joy < _max_joys )
    {
        return SDL_JoystickNumHats( _joysticks[joy] ) ;
    }
    return -1 ;
}

/* --------------------------------------------------------------------------- */
/**
 * libjoy_balls_specific (int JOY)
 * Returns the total number of balls of the specified joystick
 **/

int libjoy_balls_specific( int joy )
{
    if ( joy >= 0 && joy < _max_joys )
    {
        return SDL_JoystickNumBalls( _joysticks[joy] ) ;
    }
    return -1 ;
}

/* --------------------------------------------------------------------------- */
/**
 * libjoy_get_hat_specific (int JOY, int HAT)
 * Returns the state of the specfied hat on the specified joystick
 **/

int libjoy_get_hat_specific( int joy, int hat )
{
    if ( joy >= 0 && joy < _max_joys )
    {
        if ( hat >= 0 && hat <= SDL_JoystickNumHats( _joysticks[joy] ) )
        {
            return SDL_JoystickGetHat( _joysticks[joy], hat ) ;
        }
    }
    return -1 ;
}

/* --------------------------------------------------------------------------- */
/**
 * libjoy_get_ball_specific (int JOY, int BALL, int* dx, int* dy)
 * Returns the state of the specfied ball on the specified joystick
 **/

int libjoy_get_ball_specific( int joy, int ball, int * dx, int * dy )
{
    if ( joy >= 0 && joy < _max_joys )
    {
        if ( ball >= 0 && ball <= SDL_JoystickNumBalls( _joysticks[joy] ) )
        {
            return SDL_JoystickGetBall( _joysticks[joy], ball, dx, dy ) ;
        }
    }
    return -1 ;
}

/* --------------------------------------------------------------------------- */
/* Funciones de inicializacion del modulo/plugin                               */

DLCONSTANT __bgdexport( libjoy, constants_def )[] =
{
    { "JOY_HAT_CENTERED"    , TYPE_DWORD, SDL_HAT_CENTERED  },
    { "JOY_HAT_UP"          , TYPE_DWORD, SDL_HAT_UP        },
    { "JOY_HAT_RIGHT"       , TYPE_DWORD, SDL_HAT_RIGHT     },
    { "JOY_HAT_DOWN"        , TYPE_DWORD, SDL_HAT_DOWN      },
    { "JOY_HAT_LEFT"        , TYPE_DWORD, SDL_HAT_LEFT      },
    { "JOY_HAT_RIGHTUP"     , TYPE_DWORD, SDL_HAT_RIGHTUP   },
    { "JOY_HAT_RIGHTDOWN"   , TYPE_DWORD, SDL_HAT_RIGHTDOWN },
    { "JOY_HAT_LEFTUP"      , TYPE_DWORD, SDL_HAT_LEFTUP    },
    { "JOY_HAT_LEFTDOWN"    , TYPE_DWORD, SDL_HAT_LEFTDOWN  },
    { NULL                  , 0         , 0                 }
} ;

/* --------------------------------------------------------------------------- */

void  __bgdexport( libjoy, module_initialize )()
{
    int i;

    if ( !SDL_WasInit( SDL_INIT_JOYSTICK ) )
    {
        SDL_InitSubSystem( SDL_INIT_JOYSTICK );
        SDL_JoystickEventState( SDL_ENABLE ) ;
    }

    /* Open all joysticks */
    if (( _max_joys = SDL_NumJoysticks() ) > MAX_JOYS )
    {
        printf( "[JOY] Warning: maximum number of joysticks exceeded (%i>%i)", _max_joys, MAX_JOYS );
        _max_joys = MAX_JOYS;
    }

    for ( i = 0; i < _max_joys; i++ )
    {
        _joysticks[i] = SDL_JoystickOpen( i ) ;
        if ( !_joysticks[i] ) printf( "[JOY] Failed to open joystick '%i'", i );
    }

    SDL_JoystickUpdate() ;
}

/* ----------------------------------------------------------------- */

void  __bgdexport( libjoy, module_finalize )()
{
    int i;
    for ( i = 0; i < _max_joys; i++ )
        if ( _joysticks[i] ) SDL_JoystickClose( _joysticks[i] ) ;

    if ( SDL_WasInit( SDL_INIT_JOYSTICK ) ) SDL_QuitSubSystem( SDL_INIT_JOYSTICK );

}

/* ----------------------------------------------------------------- */

char * __bgdexport( libjoy, modules_dependency )[] =
{
    "libsdlhandler",
    NULL
};

/* ----------------------------------------------------------------- */
