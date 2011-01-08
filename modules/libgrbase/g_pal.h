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

#ifndef __PAL_H
#define __PAL_H

/* --------------------------------------------------------------------------- */
#ifdef _MSC_VER
#pragma pack(push, 1)
#endif

typedef struct _rgb_component
{
    uint8_t     r ;
    uint8_t     g ;
    uint8_t     b ;
}
#ifdef __GNUC__
__attribute__ ((packed))
#endif
rgb_component ;

typedef struct _palette
{
    rgb_component       rgb[256] ;

    uint32_t            colorequiv[256] ;

    int                 use;

    struct _palette     * next ;
    struct _palette     * prev ;
}
#ifdef __GNUC__
__attribute__ ((packed))
#endif
PALETTE ;

#ifdef _MSC_VER
#pragma pack(pop)
#endif

/* --------------------------------------------------------------------------- */

#include "g_bitmap.h"

/* --------------------------------------------------------------------------- */

/* Paleta de colores */
/* ----------------- */

extern PALETTE * first_palette ;

extern int palette_changed ;

/* Lista ordenada de colores */

extern int color_list[ 256 ] ;

/* Tabla de transparencias */

extern uint8_t trans_table[ 256 ][ 256 ] ;
extern int trans_table_updated ;

/* Deja un color a 50% de intensidad */

extern uint16_t colorghost[65536] ;

/* Paleta de colores por defecto */

extern uint8_t default_palette[];
extern uint32_t default_colorequiv[256];

/* --------------------------------------------------------------------------- */

extern int find_nearest_color( PALETTE * pal, int first, int last, int r, int g, int b );
extern int gr_map_rgb( PIXEL_FORMAT * format, int r, int g, int b );
extern int gr_map_rgba( PIXEL_FORMAT * format, int r, int g, int b, int a );
extern void pal_refresh( PALETTE * pal );
extern void pal_use( PALETTE * pal );
extern int pal_discard( PALETTE * pal );
extern PALETTE * pal_new( PALETTE * basepal );
extern PALETTE * pal_new_rgb( unsigned char * datapal );
extern void pal_destroy( PALETTE * pal );
extern int pal_get( PALETTE * spal, int color, int num, uint8_t * pal );
extern int pal_set( PALETTE * spal, int color, int num, uint8_t * pal );
extern int pal_map_assign( int libid, int mapcode, PALETTE * palid );
extern int pal_map_remove( int libid, int mapcode );
extern void gr_roll_palette( int color0, int num, int inc );
extern int gr_rgb( int r, int g, int b );
extern int gr_rgba( int r, int g, int b, int a );
extern void gr_get_rgb( int color, int *r, int *g, int *b );
extern void gr_get_rgba( int color, int *r, int *g, int *b, int *a );
extern int gr_rgb_depth( int depth, int r, int g, int b );
extern int gr_rgba_depth( int depth, int r, int g, int b, int a );
extern void gr_get_rgb_depth( int depth, int color, int *r, int *g, int *b );
extern void gr_get_rgba_depth( int depth, int color, int *r, int *g, int *b, int *a );
extern int gr_find_nearest_color( int r, int g, int b );
extern int gr_get_color( int r, int g, int b );
extern void gr_make_trans_table();
extern void gr_set_rgb( int color, int r, int g, int b );
extern void gr_get_colors( int color, int num, uint8_t * pal );
extern void gr_set_colors( int color, int num, uint8_t * pal );

/* --------------------------------------------------------------------------- */

#endif
