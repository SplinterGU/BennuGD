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
/* Thanks Sandman for suggest on openjoys at initialization time               */
/* --------------------------------------------------------------------------- */
/* Credits SplinterGU/Sandman 2007-2008                                        */
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
 * NUM_JOY ()
 * Returns the number of joysticks present in the system
 **/

static int modjoy_num( INSTANCE * my, int * params )
{
    return _max_joys ;
}

/* --------------------------------------------------------------------------- */
/**
 * JOY_NAME (int JOY)
 * Returns the name for a given joystick present in the system
 **/

static int modjoy_name( INSTANCE * my, int * params )
{
    int result;
    result = string_new( SDL_JoystickName( params[0] ) );
    string_use( result );
    return result;
}

/* --------------------------------------------------------------------------- */
/**
 * JOY_SELECT (int JOY)
 * Returns the selected joystick number
 **/

static int modjoy_select( INSTANCE * my, int * params )
{
    return ( _selected_joystick = params[0] );
}

/* --------------------------------------------------------------------------- */
/**
 * JOY_BUTTONS ()
 * Returns the selected joystick total buttons
 **/

static int modjoy_buttons( INSTANCE * my, int * params )
{
    if ( _selected_joystick >= 0 && _selected_joystick < _max_joys )
    {
        return SDL_JoystickNumButtons( _joysticks[_selected_joystick] ) ;
    }
    return -1 ;
}

/* --------------------------------------------------------------------------- */
/**
 * JOY_AXIS (int JOY)
 * Returns the selected joystick total axes
 **/

static int modjoy_axes( INSTANCE * my, int * params )
{
    if ( _selected_joystick >= 0 && _selected_joystick < _max_joys )
    {
        return SDL_JoystickNumAxes( _joysticks[_selected_joystick] ) ;
    }
    return -1 ;
}

/* --------------------------------------------------------------------------- */
/**
 * JOY_GET_BUTTON ()
 * Returns the selected joystick state for the given button
 **/

static int modjoy_get_button( INSTANCE * my, int * params )
{
    if ( _selected_joystick >= 0 && _selected_joystick < _max_joys )
    {
        return SDL_JoystickGetButton( _joysticks[_selected_joystick], params[0] ) ;
    }
    return -1 ;
}

/* --------------------------------------------------------------------------- */
/**
 * JOY_GET_POSITION ()
 * Returns the selected joystick state for the given axis
 **/

static int modjoy_get_position( INSTANCE * my, int * params )
{
    if ( _selected_joystick >= 0 && _selected_joystick < _max_joys )
    {
        return SDL_JoystickGetAxis( _joysticks[_selected_joystick], params[0] ) ;
    }
    return -1 ;
}

/* --------------------------------------------------------------------------- */
/**
 * JOY_NUMHATS ()
 * Returns the total number of POV hats of the current selected joystick
 **/

static int modjoy_hats( INSTANCE * my, int * params )
{
    if ( _selected_joystick >= 0 && _selected_joystick < _max_joys )
    {
        return SDL_JoystickNumHats( _joysticks[_selected_joystick] ) ;
    }
    return -1 ;
}

/* --------------------------------------------------------------------------- */
/**
 * JOY_NUMBALLS ()
 * Returns the total number of balls of the current selected joystick
 **/

static int modjoy_balls( INSTANCE * my, int * params )
{
    if ( _selected_joystick >= 0 && _selected_joystick < _max_joys )
    {
        return SDL_JoystickNumBalls( _joysticks[_selected_joystick] ) ;
    }
    return -1 ;
}

/* --------------------------------------------------------------------------- */
/**
 * JOY_GETHAT (int HAT)
 * Returns the state of the specfied hat on the current selected joystick
 **/

static int modjoy_get_hat( INSTANCE * my, int * params )
{
    if ( _selected_joystick >= 0 && _selected_joystick < _max_joys )
    {
        if ( params[0] >= 0 && params[0] <= SDL_JoystickNumHats( _joysticks[_selected_joystick] ) )
        {
            return SDL_JoystickGetHat( _joysticks[_selected_joystick], params[0] ) ;
        }
    }
    return -1 ;
}

/* --------------------------------------------------------------------------- */
/**
 * JOY_GETBALL (int BALL, int* dx, int* dy)
 * Returns the state of the specfied ball on the current selected joystick
 **/

static int modjoy_get_ball( INSTANCE * my, int * params )
{
    if ( _selected_joystick >= 0 && _selected_joystick < _max_joys )
    {
        if ( params[0] >= 0 && params[0] <= SDL_JoystickNumBalls( _joysticks[params[0]] ) )
        {
            return SDL_JoystickGetBall( _joysticks[_selected_joystick], params[0], ( int* )params[1], ( int* )params[2] ) ;
        }
    }
    return -1 ;
}

/* --------------------------------------------------------------------------- */
/* --------------------------------------------------------------------------- */
/* --------------------------------------------------------------------------- */

/* --------------------------------------------------------------------------- */
/**
 * JOY_BUTTONS (int JOY)
 * Returns the selected joystick total buttons
 **/

static int modjoy_buttons_specific( INSTANCE * my, int * params )
{
    if ( params[0] >= 0 && params[0] < _max_joys )
    {
        return SDL_JoystickNumButtons( _joysticks[params[0]] ) ;
    }
    return -1 ;
}

/* --------------------------------------------------------------------------- */
/**
 * JOY_AXIS (int JOY)
 * Returns the selected joystick total axes
 **/

static int modjoy_axes_specific( INSTANCE * my, int * params )
{
    if ( params[0] >= 0 && params[0] < _max_joys )
    {
        return SDL_JoystickNumAxes( _joysticks[params[0]] ) ;
    }
    return -1 ;
}

/* --------------------------------------------------------------------------- */
/**
 * JOY_GET_BUTTON (int JOY)
 * Returns the selected joystick state for the given button
 **/

static int modjoy_get_button_specific( INSTANCE * my, int * params )
{
    if ( params[0] >= 0 && params[0] < _max_joys )
    {
        if ( params[1] >= 0 && params[1] <= SDL_JoystickNumButtons( _joysticks[params[0]] ) )
        {
            return SDL_JoystickGetButton( _joysticks[params[0]], params[1] ) ;
        }
    }
    return -1 ;
}

/* --------------------------------------------------------------------------- */
/**
 * JOY_GET_POSITION (int JOY)
 * Returns the selected joystick state for the given axis
 **/

static int modjoy_get_position_specific( INSTANCE * my, int * params )
{
    if ( params[0] >= 0 && params[0] < _max_joys )
    {
        if ( params[1] >= 0 && params[1] <= SDL_JoystickNumAxes( _joysticks[params[0]] ) )
        {
            return SDL_JoystickGetAxis( _joysticks[params[0]], params[1] ) ;
        }
    }
    return -1 ;
}

/* --------------------------------------------------------------------------- */
/* Added by Sandman */
/* --------------------------------------------------------------------------- */
/**
 * JOY_NUMHATS (int JOY)
 * Returns the total number of POV hats of the specified joystick
 **/

static int modjoy_hats_specific( INSTANCE * my, int * params )
{
    if ( params[0] >= 0 && params[0] < _max_joys )
    {
        return SDL_JoystickNumHats( _joysticks[params[0]] ) ;
    }
    return -1 ;
}

/* --------------------------------------------------------------------------- */
/**
 * JOY_NUMBALLS (int JOY)
 * Returns the total number of balls of the specified joystick
 **/

static int modjoy_balls_specific( INSTANCE * my, int * params )
{
    if ( params[0] >= 0 && params[0] < _max_joys )
    {
        return SDL_JoystickNumBalls( _joysticks[params[0]] ) ;
    }
    return -1 ;
}

/* --------------------------------------------------------------------------- */
/**
 * JOY_GETHAT (int JOY, int HAT)
 * Returns the state of the specfied hat on the specified joystick
 **/

static int modjoy_get_hat_specific( INSTANCE * my, int * params )
{
    if ( params[0] >= 0 && params[0] < _max_joys )
    {
        if ( params[1] >= 0 && params[1] <= SDL_JoystickNumHats( _joysticks[params[0]] ) )
        {
            return SDL_JoystickGetHat( _joysticks[params[0]], params[1] ) ;
        }
    }
    return -1 ;
}

/* --------------------------------------------------------------------------- */
/**
 * JOY_GETBALL (int JOY, int BALL, int* dx, int* dy)
 * Returns the state of the specfied ball on the specified joystick
 **/

static int modjoy_get_ball_specific( INSTANCE * my, int * params )
{
    if ( params[0] >= 0 && params[0] < _max_joys )
    {
        if ( params[1] >= 0 && params[1] <= SDL_JoystickNumBalls( _joysticks[params[0]] ) )
        {
            return SDL_JoystickGetBall( _joysticks[params[0]], params[1], ( int* )params[2], ( int* )params[3] ) ;
        }
    }
    return -1 ;
}

/* ---------------------------------------------------------------------- */

DLSYSFUNCS  __bgdexport( mod_joy, functions_exports )[] =
{
    { "JOY_AXES"            , ""      , TYPE_INT    , modjoy_axes                  },
    { "JOY_AXES"            , "I"     , TYPE_INT    , modjoy_axes_specific         },
    { "JOY_NUMAXES"         , ""      , TYPE_INT    , modjoy_axes                  },
    { "JOY_NUMAXES"         , "I"     , TYPE_INT    , modjoy_axes_specific         },
    { "JOY_GETAXIS"         , "I"     , TYPE_INT    , modjoy_get_position          },
    { "JOY_GETAXIS"         , "II"    , TYPE_INT    , modjoy_get_position_specific },

    { "JOY_BUTTONS"         , ""      , TYPE_INT    , modjoy_buttons               },
    { "JOY_BUTTONS"         , "I"     , TYPE_INT    , modjoy_buttons_specific      },

    { "JOY_NAME"            , "I"     , TYPE_STRING , modjoy_name                  },

    { "JOY_NUMBUTTONS"      , ""      , TYPE_INT    , modjoy_buttons               },
    { "JOY_NUMBUTTONS"      , "I"     , TYPE_INT    , modjoy_buttons_specific      },

    { "JOY_NUMBER"          , ""      , TYPE_INT    , modjoy_num                   },
    { "JOY_NUMJOYSTICKS"    , ""      , TYPE_INT    , modjoy_num                   },

    { "JOY_SELECT"          , "I"     , TYPE_INT    , modjoy_select                },

    { "JOY_GETBUTTON"       , "I"     , TYPE_INT    , modjoy_get_button            },
    { "JOY_GETBUTTON"       , "II"    , TYPE_INT    , modjoy_get_button_specific   },
    { "JOY_GETPOSITION"     , "I"     , TYPE_INT    , modjoy_get_position          },
    { "JOY_GETPOSITION"     , "II"    , TYPE_INT    , modjoy_get_position_specific },


    { "JOY_NUMHATS"         , ""      , TYPE_INT    , modjoy_hats                  },
    { "JOY_NUMHATS"         , "I"     , TYPE_INT    , modjoy_hats_specific         },   /* Added by Sandman */
    { "JOY_NUMBALLS"        , ""      , TYPE_INT    , modjoy_balls                 },
    { "JOY_NUMBALLS"        , "I"     , TYPE_INT    , modjoy_balls_specific        },   /* Added by Sandman */
    { "JOY_GETHAT"          , "I"     , TYPE_INT    , modjoy_get_hat               },
    { "JOY_GETHAT"          , "II"    , TYPE_INT    , modjoy_get_hat_specific      },   /* Added by Sandman */
    { "JOY_GETBALL"         , "IPP"   , TYPE_INT    , modjoy_get_ball              },
    { "JOY_GETBALL"         , "IIPP"  , TYPE_INT    , modjoy_get_ball_specific     },   /* Added by Sandman */

    /* Compatibility */

    { "NUMBER_JOY"          , ""      , TYPE_INT    , modjoy_num                   },
    { "SELECT_JOY"          , "I"     , TYPE_INT    , modjoy_select                },
    { "GET_JOY_BUTTON"      , "I"     , TYPE_INT    , modjoy_get_button            },
    { "GET_JOY_BUTTON"      , "II"    , TYPE_INT    , modjoy_get_button_specific   },
    { "GET_JOY_POSITION"    , "I"     , TYPE_INT    , modjoy_get_position          },
    { "GET_JOY_POSITION"    , "II"    , TYPE_INT    , modjoy_get_position_specific },
    { 0                     , 0       , 0           , 0                             }
};

/* --------------------------------------------------------------------------- */

DLCONSTANT __bgdexport( mod_joy, constants_def )[] =
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

/* ----------------------------------------------------------------- */
/* Funciones de inicializacion del modulo/plugin                     */

void  __bgdexport( mod_joy, module_initialize )()
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

void  __bgdexport( mod_joy, module_finalize )()
{
    int i;
    for ( i = 0; i < _max_joys; i++ )
        if ( _joysticks[i] ) SDL_JoystickClose( _joysticks[i] ) ;

    if ( SDL_WasInit( SDL_INIT_JOYSTICK ) ) SDL_QuitSubSystem( SDL_INIT_JOYSTICK );

}

/* --------------------------------------------------------------------------- */
