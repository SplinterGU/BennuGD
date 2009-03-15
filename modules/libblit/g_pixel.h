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

#ifndef __PIXEL_H
#define __PIXEL_H

/* --------------------------------------------------------------------------- */

#include <stdint.h>

/* --------------------------------------------------------------------------- */

extern int pixel_color8 ;
extern uint16_t pixel_color16 ;
extern uint32_t pixel_color32 ;

extern uint16_t pixel_color16_alpha ;
extern uint32_t pixel_color32_alpha ;

extern int pixel_alpha ;

extern uint8_t * pixel_alpha8 ;
extern uint16_t * pixel_alpha16 ;

/* --------------------------------------------------------------------------- */
/* Pixels */
/* ------ */

/* --------------------------------------------------------------------------- */

#define _Pixel8(ptr,color)              * ((uint8_t *)(ptr)) = (( pixel_alpha == 255 ) ? (color) : (pixel_alpha8[(uint8_t)(((color) << 8) + *(ptr))]))
#define _Pixel16(ptr,color,color_alpha) * ((uint16_t *)(ptr)) = (( pixel_alpha == 255 ) ? (color) : (pixel_alpha16[(uint16_t)*(ptr)] + (color_alpha)))
#define _Pixel32(ptr,color)             * ((uint32_t *)(ptr)) = (( pixel_alpha == 255 ) ? (color) : (color))

/* --------------------------------------------------------------------------- */

extern int gr_get_pixel( GRAPH * dest, int x, int y );
extern void gr_put_pixel( GRAPH * dest, int x, int y, int color );
extern void gr_put_pixelc( GRAPH * dest, REGION * clip, int x, int y, int color );
extern void gr_setalpha( int value );
extern void gr_setcolor( int c );

/* --------------------------------------------------------------------------- */
#endif
