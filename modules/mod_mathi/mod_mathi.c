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

/* --------------------------------------------------------------------------- */

#define _GNU_SOURCE
#include <math.h>

#include <stdio.h>

#include "bgddl.h"

/* --------------------------------------------------------------------------- */

DLCONSTANT __bgdexport( mod_mathi, constants_def )[] =
{
    { "PI"  , TYPE_INT  , 180000    },
    { NULL  , 0         , 0         }
} ;

/* --------------------------------------------------------------------------- */
/* Funciones matemáticas */
/* --------------------------------------------------------------------------- */

static int math_abs( INSTANCE * my, int * params )
{
    float num = *( float * ) &params[0] ;
    float res = ( num < 0 ) ? -num : num ;
    return *(( int * )&res ) ;
}

/* --------------------------------------------------------------------------- */

static int math_pow( INSTANCE * my, int * params )
{
    float res = ( float )pow( *( float * ) & params[0], *( float * ) & params[1] ) ;
    return *(( int * )&res ) ;
}

/* --------------------------------------------------------------------------- */

static int math_sqrt( INSTANCE * my, int * params )
{
    float res = ( float )sqrt( *( float * ) & params[0] ) ;
    return *(( int * )&res ) ;
}

/* --------------------------------------------------------------------------- */

static int math_cos( INSTANCE * my, int * params )
{
    return cos(( double )( params[0] * M_PI / 180000.0 )) * 1000 ;
}

/* --------------------------------------------------------------------------- */

static int math_sin( INSTANCE * my, int * params )
{
    return sin(( double )( params[0] * M_PI / 180000.0 )) * 1000 ;
}

/* --------------------------------------------------------------------------- */

static int math_tan( INSTANCE * my, int * params )
{
    return tan(( double )( params[0] * M_PI / 180000.0 )) * 1000 ;
}

/* --------------------------------------------------------------------------- */

static int math_acos( INSTANCE * my, int * params )
{
    return acos(( double )( params[0] * 180000.0 / M_PI )) * 1000 ;
}

/* --------------------------------------------------------------------------- */

static int math_asin( INSTANCE * my, int * params )
{
    return asin(( double )( params[0] * 180000.0 / M_PI )) * 1000 ;
}

/* --------------------------------------------------------------------------- */

static int math_atan( INSTANCE * my, int * params )
{
    return atan(( double )( params[0] * 180000.0 / M_PI )) * 1000 ;
}

/* --------------------------------------------------------------------------- */

static int math_atan2( INSTANCE * my, int * params )
{
    return atan2(( double )( params[0] * 180000.0 / M_PI ), ( double )( params[1] * 180000.0 / M_PI )) * 1000 ;
}

/* --------------------------------------------------------------------------- */

static int math_isinf( INSTANCE * my, int * params )
{
    double param = ( double ) *( float * ) & params[0] ;
    return isinf( param );
}

/* --------------------------------------------------------------------------- */

static int math_isnan( INSTANCE * my, int * params )
{
    double param = ( double ) *( float * ) & params[0] ;
    return isnan( param );
}

/* --------------------------------------------------------------------------- */

static int math_finite( INSTANCE * my, int * params )
{
    double param = ( double ) *( float * ) & params[0] ;
    return finite ( param );
}

/* --------------------------------------------------------------------------- */

static int math_get_disty( INSTANCE * my, int * params )
{
    double angle = params[0] * M_PI / 180000.0 ;
    return ( int )( params[1] * -sin( angle ) ) ;
}

/* --------------------------------------------------------------------------- */

static int math_fget_angle( INSTANCE * my, int * params )
{
    double dx = params[2] - params[0] ;
    double dy = params[3] - params[1] ;
    int angle ;

    if ( dx == 0 ) return ( dy > 0 ) ? 270000L : 90000L ;

    angle = ( int )( atan( dy / dx ) * 180000.0 / M_PI ) ;

    return ( dx > 0 ) ? -angle : -angle + 180000L ;
}

/* --------------------------------------------------------------------------- */

static int math_fget_dist( INSTANCE * my, int * params )
{
    double dx = params[2] - params[0] ;
    double dy = params[3] - params[1] ;

    return ( int )sqrt( dx*dx + dy*dy ) ;
}

/* --------------------------------------------------------------------------- */

static int math_near_angle( INSTANCE * my, int * params )
{
    int angle = params[0] ;
    int dest  = params[1] ;
    int incr  = params[2] ;

    if ( angle < dest && dest - angle > 180000L )
        angle += 360000L ;

    if ( angle > dest && angle - dest > 180000L )
        angle -= 360000L ;

    if ( angle < dest )
    {
        angle += incr ;
        if ( angle > dest ) angle = dest ;
    }
    else
    {
        angle -= incr ;
        if ( angle < dest ) angle = dest ;
    }

    if ( angle < 0 ) return angle + 360000L ;
    if ( angle >= 360000L ) return angle - 360000L ;
    return angle ;
}

/* --------------------------------------------------------------------------- */

static int math_get_distx( INSTANCE * my, int * params )
{
    double angle = params[0] * M_PI / 180000.0 ;
    return ( int )( params[1] * cos( angle ) ) ;
}

/* --------------------------------------------------------------------------- */
/* Declaracion de funciones                                                    */

DLSYSFUNCS __bgdexport( mod_mathi, functions_exports )[] =
{
    { "ABS"         , "F"       , TYPE_FLOAT    , math_abs          },
    { "POW"         , "FF"      , TYPE_FLOAT    , math_pow          },
    { "SQRT"        , "F"       , TYPE_FLOAT    , math_sqrt         },

    { "COS"         , "I"       , TYPE_FLOAT    , math_cos          },
    { "SIN"         , "I"       , TYPE_FLOAT    , math_sin          },
    { "TAN"         , "I"       , TYPE_FLOAT    , math_tan          },
    { "ACOS"        , "I"       , TYPE_FLOAT    , math_acos         },
    { "ASIN"        , "I"       , TYPE_FLOAT    , math_asin         },
    { "ATAN"        , "I"       , TYPE_FLOAT    , math_atan         },
    { "ATAN2"       , "II"      , TYPE_FLOAT    , math_atan2        },

    { "ISINF"       , "F"       , TYPE_INT      , math_isinf        },
    { "ISNAN"       , "F"       , TYPE_INT      , math_isnan        },
    { "FINITE"      , "F"       , TYPE_INT      , math_finite       },

    { "FGET_ANGLE"  , "IIII"    , TYPE_INT      , math_fget_angle   },
    { "FGET_DIST"   , "IIII"    , TYPE_INT      , math_fget_dist    },
    { "NEAR_ANGLE"  , "III"     , TYPE_INT      , math_near_angle   },
    { "GET_DISTX"   , "II"      , TYPE_INT      , math_get_distx    },
    { "GET_DISTY"   , "II"      , TYPE_INT      , math_get_disty    },

    { 0             , 0         , 0             , 0                 }
};

/* --------------------------------------------------------------------------- */
