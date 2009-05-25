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

#ifndef __BLEND_H
#define __BLEND_H

/* Blend ops */
/* --------- */

extern void blend_apply( GRAPH * bitmap, int16_t * blend ) ;
extern void blend_assign( GRAPH * bitmap, int16_t * blend ) ;
extern int16_t * blend_create( void ) ;
extern void blend_free( int16_t * blend );
extern void blend_grayscale( int16_t * blend, int method ) ;
extern void blend_init( int16_t * blend ) ;
extern void blend_intensity( int16_t * blend, float ammount ) ;
extern void blend_swap( int16_t * blend ) ;
extern void blend_tint( int16_t * blend, float ammount, uint8_t cr, uint8_t cg, uint8_t cb ) ;
extern void blend_translucency( int16_t * blend, float ammount ) ;

#endif
