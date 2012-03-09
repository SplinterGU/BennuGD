/*
 *  Copyright © 2006-2012 SplinterGU (Fenix/Bennugd)
 *  Copyright © 2002-2006 Fenix Team (Fenix)
 *  Copyright © 1999-2002 José Luis Cebrián Pagüe (Fenix)
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

#ifndef _FMATH_H
#define _FMATH_H

#include <math.h>

#include "bgdcore.h"

/* Rutinas matemáticas de punto fijo, basadas en Allegro */

#ifndef M_PI
#define M_PI    3.14159265358979323846  /* pi */
#endif

#ifndef ABS
#define ABS(x) (((x) < 0) ? -(x):(x))
#endif
#ifndef SGN
#define SGN(a)  (((a) < 0) ? -1 : ((a) > 0) ? 1 : 0)
#endif

typedef long int fixed ;

#ifndef __GNUC__
#define inline __inline
#endif

#ifdef DEBUG
# define __INLINE static
#else
# define __INLINE static inline
#endif

//#define FIXED_PREC 12

#define FIXED_PREC      10000
//#define FIXED_PREC_MED  5000
//#define FIXED_PREC_DEC  1000

static fixed * cos_table = NULL ;

__INLINE fixed ftofix( float x )
{
    return ( long )( x * FIXED_PREC );

}

__INLINE float fixtof( fixed x )
{
    return (( float )x ) / FIXED_PREC ;
}

__INLINE fixed itofix( int x )
{
    return x * FIXED_PREC ;
}

__INLINE int fixtoi( fixed x )
{
    return x / FIXED_PREC ;
}

__INLINE int fixceil( fixed x )
{
    int xd;

    if ( x < 0 )
    {
        xd = x % FIXED_PREC ;
        x -= ( FIXED_PREC + xd ) ;
    }
    else if ( x > 0 )
    {
        xd = x % FIXED_PREC ;
        x += ( FIXED_PREC - xd ) ;
    }

    return x ;
}

__INLINE fixed fcos( int x )
{
    if ( x < 0 ) x = -x ;
    if ( x > 360000 ) x %= 360000 ;
    if ( x > 270000 ) return cos_table[360000 - x] ;
    if ( x > 180000 ) return -cos_table[x - 180000] ;
    if ( x > 90000 ) return -cos_table[180000 - x] ;
    return cos_table[x] ;
}

__INLINE fixed fsin( int x )
{
    if ( x < 0 ) return -fsin( -x ) ;
    if ( x > 360000 ) x %= 360000 ;
    if ( x > 270000 ) return -cos_table[x - 270000] ;
    if ( x > 180000 ) return -cos_table[270000 - x] ;
    if ( x > 90000 ) return cos_table[x - 90000] ;
    return cos_table[90000 - x] ;
}

__INLINE fixed fmul( int x, int y )
{
    return ftofix( fixtof( x ) * fixtof( y ) ) ;
}

__INLINE fixed fdiv( int x, int y )
{
    return ftofix( fixtof( x ) / fixtof( y ) ) ;
}

static void init_cos_tables()
{
    int i ;

    if ( !cos_table ) cos_table = ( fixed * ) malloc( 90001 * sizeof( fixed ) );

    for ( i = 0 ; i <= 90000 ; i++ )
    {
        cos_table[i] = ftofix( cos( i * M_PI / 180000.0 ) ) ;
    }
}

#endif
