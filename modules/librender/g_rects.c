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

#define __LIB_RENDER
#include "librender.h"
#include "fmath.h"

/* --------------------------------------------------------------------------- */

uint8_t zonearray[ 128 / 8 ];

/* --------------------------------------------------------------------------- */

void gr_rects_clear()
{
    memset( zonearray, 0, sizeof( zonearray ) );
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : gr_mark_rect
 *
 *  Updates the given rectangle as a dirty zone at the 128-bits array
 *
 *  PARAMS :
 *      x, y                Top-left coordinate
 *      width               Width in pixels
 *      height              Height in pixels
 *
 *  RETURN VALUE :
 *      None
 */

void gr_mark_rect( int x, int y, int width, int height )
{
    int cx, cy;
    int w, h;
    int iy, lx, ly;

    w = scr_width / 16;
    h = scr_height / 8;

    width = ABS( width ) - 1;
    height = ABS( height ) - 1;

    x = MIN( x, x + width );
    y = MIN( y, y + height );

    iy = MAX( y / h, 0 );

    lx = MIN(( x + width ) / w, 15 );
    ly = MIN(( y + height ) / h, 7 );

    for ( cx = MAX( x / w, 0 ); cx <= lx; cx++ )
        for ( cy = iy; cy <= ly; cy++ )
            zonearray[ cx ] |= ( 1 << cy );
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : gr_mark_rects
 *
 *  Given a 128-bits array of dirty screen zones, create an array of SDL_Rect regions
 *
 *  PARAMS :
 *      zonearray       Pointer to a 128-bits array with dirty screen zones = 1
 *      rects           Pointer to a 128 REGION array
 *
 *  RETURN VALUE :
 *      Number of rects filled
 */

int gr_mark_rects( REGION * rects )
{
    int count = 0, x, y;
    int w, h, cw, ch, x2;

    w = scr_width / 16;
    h = scr_height / 8;

    for ( x = 0 ; x < 16 ; x++ )
    {
        if ( zonearray[ x ] )
        {
            for ( y = 0 ; y < 8 ; y++ )
            {
                if ( zonearray[ x ] & ( 1 << y ) )
                {
                    zonearray[ x ] &= ~( 1 << y );
                    for ( cw = x + 1; ( cw < 16 ) && ( zonearray[ cw ] & ( 1 << y ) ); cw++ )
                        zonearray[ cw ] &= ~( 1 << y );

                    for ( ch = y + 1 ; ch < 8 ; ch++ )
                    {
                        /* Si hay algun hueco en el ancho de las siguiente lineas, corto aca,
                           y deja esta linea para otra recta */
                        for ( x2 = x ; ( x2 < cw ) && ( zonearray[ x2 ] & ( 1 << ch ) ) ; x2++ ) ;

                        if ( x2 < cw ) break;

                        /* Limpio bitmap de la recta actual */
                        for ( x2 = x ; x2 < cw ; x2++ )
                            zonearray[ x2 ] &= ~( 1 << ch );
                    }
                    rects[ count ].x = w * x ;
                    rects[ count ].y = h * y ;
                    rects[ count ].x2 = w * cw - 1 /* + rects[ count ].x */;
                    rects[ count ].y2 = h * ch - 1 /* + rects[ count ].y */;
                    count++;
                }
            }
        }
    }
    return count;
}

/* --------------------------------------------------------------------------- */
