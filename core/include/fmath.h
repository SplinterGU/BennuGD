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
