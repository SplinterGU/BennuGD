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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libgrbase.h"

/* --------------------------------------------------------------------------- */

uint32_t * map_code_bmp = NULL ;
int map_code_allocated = 0 ;
int map_code_last = 0;

GRAPH * map_first = NULL;

/* static int free_map_code = 1000 ; */

#define bit_set(m,b)    (((uint32_t *)(m))[(b)>>5] |=   1<<((b)&0x1F))
#define bit_clr(m,b)    (((uint32_t *)(m))[(b)>>5] &= ~(1<<((b)&0x1F)))
#define bit_tst(m,b)    (((uint32_t *)(m))[(b)>>5] &   (1<<((b)&0x1F)))

/* --------------------------------------------------------------------------- */

PIXEL_FORMAT * bitmap_create_format( int bpp )
{
    PIXEL_FORMAT *format;

    /* Allocate an empty pixel format structure */
    format = malloc( sizeof( *format ) );
    if ( !format ) return( NULL );

    memset( format, 0, sizeof( *format ) );

    /* Set up the format */

    format->depth = bpp;
    format->depthb = ( bpp + 7 ) / 8;

    if ( bpp == 32 )
    {
        format->Aloss = 0;
        format->Rloss = 0;
        format->Gloss = 0;
        format->Bloss = 0;

        format->Ashift = 24;
        format->Rshift = 16;
        format->Gshift = 8;
        format->Bshift = 0;

        format->Amask = 0xFF000000;
        format->Rmask = 0x00FF0000;
        format->Gmask = 0x0000FF00;
        format->Bmask = 0x000000FF;
    }
    else if ( bpp > 8 )
    {
        /* R-G-B */
        if ( bpp > 24 ) bpp = 24;

        format->Rloss = 8 - ( bpp / 3 );
        format->Gloss = 8 - ( bpp / 3 ) - ( bpp % 3 );
        format->Bloss = 8 - ( bpp / 3 );

        format->Rshift = (( bpp / 3 ) + ( bpp % 3 ) ) + ( bpp / 3 );
        format->Gshift = ( bpp / 3 );
        format->Bshift = 0;

        format->Rmask = (( 0xFF >> format->Rloss ) << format->Rshift );
        format->Gmask = (( 0xFF >> format->Gloss ) << format->Gshift );
        format->Bmask = (( 0xFF >> format->Bloss ) << format->Bshift );
    }
    else
    {
        format->Rloss = 8;
        format->Gloss = 8;
        format->Bloss = 8;
        format->Aloss = 8;

        format->Rshift = 0;
        format->Gshift = 0;
        format->Bshift = 0;
        format->Ashift = 0;

        format->Rmask = 0;
        format->Gmask = 0;
        format->Bmask = 0;
        format->Amask = 0;
    }

    return( format );
}

/* --------------------------------------------------------------------------- */

GRAPH * bitmap_new_ex( int code, int w, int h, int depth, void * data, int pitch )
{
    GRAPH * gr ;
    int wb ;

//    if ( depth != 8 && depth != 16 && depth != 1 ) return NULL; // Profundidad de color no soportada

    /* Create and fill the struct */

    gr = ( GRAPH * ) malloc( sizeof( GRAPH ) ) ;
    if ( !gr ) return NULL; // sin memoria

    /* Calculate the row size (dword-aligned) */

    wb = w * depth / 8;
    if (( wb * 8 / depth ) < w ) wb++;

    gr->data = data ;

    gr->width = w ;
    gr->height = h ;

    gr->format = bitmap_create_format( depth ) ;

    gr->pitch = pitch ;
    gr->widthb = wb ;

    gr->code = code ;
    gr->name[ 0 ] = '\0';

    gr->ncpoints = 0;
    gr->cpoints = NULL ;

    gr->format->palette = NULL ;

    gr->blend_table = NULL ;

    gr->modified = 0;
    gr->info_flags = GI_EXTERNAL_DATA ;

    gr->next = map_first;
    gr->prev = NULL;

    if ( map_first ) map_first->prev = gr;

    map_first = gr;

    return gr ;
}

/* --------------------------------------------------------------------------- */

GRAPH * bitmap_new( int code, int w, int h, int depth )
{
    GRAPH * gr ;
    int bytesPerRow, wb ;

//    if ( depth != 8 && depth != 16 && depth != 1 ) return NULL; // Profundidad de color no soportada

    /* Create and fill the struct */

    gr = ( GRAPH * ) malloc( sizeof( GRAPH ) ) ;
    if ( !gr ) return NULL; // sin memoria

    /* Calculate the row size (dword-aligned) */

    wb = w * depth / 8;
    if (( wb * 8 / depth ) < w ) wb++;

    bytesPerRow = wb;
    if ( bytesPerRow & 0x03 ) bytesPerRow = ( bytesPerRow & ~3 ) + 4;

    gr->data = ( char * ) malloc( h * bytesPerRow ) ;
    if ( !gr->data )   // Sin memoria
    {
        free( gr );
        return NULL;
    }

    gr->width = w ;
    gr->height = h ;

    gr->format = bitmap_create_format( depth ) ;

    gr->pitch = bytesPerRow ;
    gr->widthb = wb ;

    gr->code = code ;
    gr->name[ 0 ] = '\0';

    gr->ncpoints = 0;
    gr->cpoints = NULL ;

    gr->format->palette = NULL ;

    gr->blend_table = NULL ;

    gr->modified = 0;
    gr->info_flags = 0;

    gr->next = map_first;
    gr->prev = NULL;

    if ( map_first ) map_first->prev = gr;

    map_first = gr;

    return gr ;
}

/* --------------------------------------------------------------------------- */

GRAPH * bitmap_clone( GRAPH * map )
{
    GRAPH * gr ;
    uint32_t y;

    gr = bitmap_new( 0, map->width, map->height, map->format->depth ) ;
    if ( gr == NULL ) return NULL;

    for ( y = 0 ; y < map->height ; y++ )
        memcpy(( uint8_t* ) gr->data + gr->pitch * y, ( uint8_t* ) map->data + gr->pitch * y, gr->widthb );

    if ( map->cpoints )
    {
        gr->cpoints = malloc( sizeof( CPOINT ) * map->ncpoints ) ;
        memcpy( gr->cpoints, map->cpoints, sizeof( CPOINT ) * map->ncpoints ) ;
        gr->ncpoints = map->ncpoints ;
    }

    gr->blend_table = map->blend_table;
    gr->info_flags = map->info_flags ;
    gr->modified = map->modified ;
    gr->format->palette = map->format->palette ;
    pal_use( map->format->palette );

    memcpy( gr->name, map->name, sizeof( map->name ) ) ;

    return gr ;
}

/* --------------------------------------------------------------------------- */

void bitmap_add_cpoint( GRAPH * map, int x, int y )
{
    map->cpoints = ( CPOINT * ) realloc( map->cpoints, ( map->ncpoints + 1 ) * sizeof( CPOINT ) ) ;
    map->cpoints[ map->ncpoints ].x = x ;
    map->cpoints[ map->ncpoints ].y = y ;
    map->ncpoints++;
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : bitmap_set_cpoint
 *
 *  Set a control point in a graphic
 *
 *  PARAMS :
 *      map             Pointer to the bitmap
 *      point           Control point index
 *      x               New X coordinate or CPOINT_UNDEFINED to unset
 *      y               New Y coordinate or CPOINT_UNDEFINED to unset
 *
 *  RETURN VALUE :
 *      None
 *
 */

void bitmap_set_cpoint( GRAPH * map, uint32_t point, int x, int y )
{
    uint32_t n;

    if ( point < 0 ) return ;

    if ( point == 0 ) map->modified = 1;

    if ( map->ncpoints <= point )
    {
        map->cpoints = ( CPOINT * ) realloc( map->cpoints, ( point + 1 ) * sizeof( CPOINT ) ) ;
        for ( n = map->ncpoints; n < point; n++ )
        {
            map->cpoints[ n ].x = CPOINT_UNDEFINED;
            map->cpoints[ n ].y = CPOINT_UNDEFINED;
        }
        map->ncpoints = point + 1 ;
    }
    map->cpoints[ point ].x = x;
    map->cpoints[ point ].y = y;
}

/* --------------------------------------------------------------------------- */

void bitmap_destroy( GRAPH * map )
{
    if ( !map ) return ;

    if ( map->prev ) map->prev->next = map->next;
    if ( map->next ) map->next->prev = map->prev;
    if ( map_first == map ) map_first = map->next;

    if ( map->cpoints ) free( map->cpoints ) ;

    pal_destroy( map->format->palette );

    if ( map->code > 999 ) bit_clr( map_code_bmp, map->code - 1000 );

    if ( map->data && !( map->info_flags & GI_EXTERNAL_DATA ) ) free( map->data ) ;
    free( map ) ;
}

/* --------------------------------------------------------------------------- */
/* Análisis */

void bitmap_analize( GRAPH * bitmap )
{
    uint32_t x, y;

    if ( bitmap->modified > 1 ) bitmap->modified = 1 ;

    bitmap->info_flags &= ~GI_ANALIZE_MASK ;

    /* Search for transparent pixels (value 0).
     * If none found, set the flag GI_NOCOLORKEY */

    switch ( bitmap->format->depth )
    {
        case    8:
        {
            uint8_t * ptr = ( int8_t * ) bitmap->data ;

            for ( y = bitmap->height; y--; ptr += bitmap->pitch )
            {
                if ( memchr( ptr, 0, bitmap->width ) ) break ;
            }
        }
        break;
        case    16:
        {
            int16_t * ptr = ( int16_t * ) bitmap->data ;
            int inc = bitmap->pitch - bitmap->widthb ;

            for ( y = bitmap->height; y--; ptr = ( int16_t * )(( uint8_t * ) ptr ) + inc )
            {
                for ( x = bitmap->width; x--; ) if ( !*ptr++ ) break;
                if ( x < 0 ) break;
            }
        }
        break;
        case    32:
        {
            int32_t * ptr = ( int32_t * ) bitmap->data ;
            int inc = bitmap->pitch - bitmap->widthb ;

            for ( y = bitmap->height; y--; ptr = ( int32_t * )(( uint8_t * ) ptr ) + inc )
            {
                for ( x = bitmap->width; x--; ) if ( !*ptr++ ) break;
                if ( x < 0 ) break;
            }
        }
    }
    if ( y < 0 ) bitmap->info_flags |= GI_NOCOLORKEY ;
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : bitmap_16bits_conversion
 *
 *  When 16 bits mode is initialized for the first time, this
 *  function will convert every 16 bit bitmap in memory to
 *  the screen format
 *
 *  PARAMS :
 *      None
 *
 *  RETURN VALUE :
 *      None
 *
 */

void bitmap_16bits_conversion()
{
    GRAPH * map = map_first;

    while ( map )
    {
        if ( map->format->depth == 16 )
            gr_convert16_565ToScreen( map->data, map->width * map->height );

        map = map->next;
    }
}

/* --------------------------------------------------------------------------- */
/* Returns the code of a new system library graph (1000+). Searchs
   for free slots if the program creates too many system maps */

int bitmap_next_code()
{
    int n, nb, lim, ini ;

    // Si tengo suficientes alocados, retorno el siguiente segun map_code_last
    if ( map_code_last < map_code_allocated )
    {
        if ( !bit_tst( map_code_bmp, map_code_last ) )
        {
            bit_set( map_code_bmp, map_code_last );
            return 1000 + map_code_last++ ;
        }
    }

    // Ya no tengo mas espacio, entonces busco alguno libre entre ~+32 desde el ultimo fijo y ~-32 del ultimo asignado

    ini = ( map_code_last < map_code_allocated ) ? ( map_code_last >> 5 ) : 0 ;
    lim = ( map_code_allocated >> 5 ) ;

    while ( 1 )
    {
        for ( n = ini; n < lim ; n++ )
        {
            if ( map_code_bmp[n] != 0xFFFFFFFF ) // Aca hay 1 libre, busco cual es
            {
                for ( nb = 0; nb < 32; nb++ )
                {
                    if ( !bit_tst( map_code_bmp + n, nb ) )
                    {
                        map_code_last = ( n << 5 ) + nb ;
                        bit_set( map_code_bmp, map_code_last );
                        return 1000 + map_code_last++ ;
                    }
                }
            }
        }
        if ( !ini ) break;
        lim = ini;
        ini = 0;
    }

    map_code_last = map_code_allocated ;

    // Increment space, no more slots availables
    // 256 new maps available for alloc

    map_code_allocated += 256 ;
    map_code_bmp = ( uint32_t * ) realloc( map_code_bmp, sizeof( uint32_t ) * ( map_code_allocated >> 5 ) );

    memset( &map_code_bmp[( map_code_last >> 5 )], 0, 8 );  // 256 >> 5 = 8

    // Devuelvo map_code_last e incremento en 1, ya que ahora tengo BLOCK_INCR mas que antes
    bit_set( map_code_bmp, map_code_last );
    return 1000 + map_code_last++ ;

}

/* --------------------------------------------------------------------------- */

GRAPH * bitmap_new_syslib( int w, int h, int depth )
{
    GRAPH * gr ;

    if ( !syslib ) return NULL;

    gr = bitmap_new( 0, w, h, depth ) ;
    if ( !gr ) return NULL;

    gr->code = bitmap_next_code() ;
    grlib_add_map( 0, gr ) ;

    return gr ;
}

