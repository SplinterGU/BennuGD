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

#include <string.h>

#include <math.h>
#include <stdlib.h>

#include "bgddl.h"

#include "libgrbase.h"

/* --------------------------------------------------------------------------- */

#define BLUR_NORMAL     0
#define BLUR_3x3        1
#define BLUR_5x5        2
#define BLUR_5x5_MAP    3

#define GSCALE_RGB      0
#define GSCALE_R        1
#define GSCALE_G        2
#define GSCALE_B        3
#define GSCALE_RG       4
#define GSCALE_RB       5
#define GSCALE_GB       6
#define GSCALE_OFF     -1

/* --------------------------------------------------------------------------- */
/* Esto podria/deberia estar en una liberia general */

#ifdef __GNUC__
#define _inline inline
#endif

_inline void _Pixel8( uint8_t * ptr, int color )
{
    *ptr = color ;
}

_inline void _Pixel16( uint16_t * ptr, uint16_t color, uint16_t color_alpha )
{
    *ptr = color ;
}

/*
 *  FUNCTION : _get_pixel
 *
 *  Read a pixel from a bitmap
 *
 *  PARAMS :
 *      dest            Destination bitmap
 *      x, y            Pixel coordinates
 *
 *  RETURN VALUE :
 *      1, 8 or 16-bit integer with the pixel value
 *
 */

static int _get_pixel( GRAPH * dest, int x, int y )
{
    if ( x < 0 || y < 0 || x >= ( int )dest->width || y >= ( int )dest->height ) return -1 ;

    switch ( dest->format->depth )
    {
        case 8:
            return (( uint8_t * )dest->data )[x + dest->pitch * y] ;

        case 16:
            return (( uint16_t * )dest->data )[x + dest->pitch * y / 2] ;

        case 1:
            return ((( uint8_t * )dest->data )[x / 8 + dest->pitch * y] & ( 0x80 >> ( x & 7 ) ) ) ? 1 : 0;
    }
    return -1;
}

/*
 *  FUNCTION : _put_pixel
 *
 *  Paint a pixel with no clipping whatsoever, except by
 *  the bitmap's dimensions
 *
 *  PARAMS :
 *      dest            Destination bitmap
 *      x, y            Pixel coordinates
 *      color           1, 8 or 16-bit pixel value
 *
 *  RETURN VALUE :
 *      None
 *
 */

static void _put_pixel( GRAPH * dest, int x, int y, int color )
{
    if ( x < 0 || y < 0 || x >= ( int )dest->width || y >= ( int )dest->height ) return ;

    dest->modified = 2 ;

    switch ( dest->format->depth )
    {
        case 8:
            _Pixel8((( uint8_t * )dest->data ) + x + dest->pitch * y, color );
            break;

        case 16:
            _Pixel16((( uint16_t * )dest->data ) + x + dest->pitch * y / 2, color, 0 ) ;
            break;

        case 1:
            if ( color )
                ((( uint8_t * )dest->data )[x / 8 + dest->pitch * y] ) |= ( 0x80 >> ( x & 7 ) ) ;
            else
                ((( uint8_t * )dest->data )[x / 8 + dest->pitch * y] ) &= ~( 0x80 >> ( x & 7 ) ) ;
            break;
    }
}

/* ----------------------------------------------------------------- */

static int modeffects_filter( INSTANCE *my, int *params )
{ //fpg,map,tabla10

    GRAPH * map = bitmap_get( params[0], params[1] ), * map2;
    int *tabla = ( int* )params[2];
    int x, y, i, j;
    int r, g, b, r2, g2, b2, c;
    float r1, g1, b1;
    int color;

    if ( !map ) return 0;
    if ( map->format->depth != 16 ) return 0;

    map2 = bitmap_clone( map );

    r1 = 0;
    g1 = 0;
    b1 = 0;
    c = 0;
    for ( i = 0;i < map->width;i++ )
    {
        for ( j = 0;j < map->height;j++ )
        {
            color = _get_pixel( map, i, j );
            if ( !color ) continue;

            for ( y = j - 1;y < j + 2;y++ )
            {
                r2 = 0;
                g2 = 0;
                b2 = 0;
                for ( x = i - 1;x < i + 2;x++, c++ )
                {
                    color = _get_pixel( map, ( x < 0 ) ? 0 : ( x > map->width - 1 ) ? map->width - 1 : x, ( y < 0 ) ? 0 : ( y > map->height - 1 ) ? map->height - 1 : y );
                    if ( !color )
                    {
                        /* Si es transparente, repetimos el ultimo color */
                        r1 += ( float )( r2 * tabla[c] );
                        g1 += ( float )( g2 * tabla[c] );
                        b1 += ( float )( b2 * tabla[c] );
                        continue;
                    }

                    gr_get_rgb( color, &r2, &g2, &b2 );

                    r1 += ( float )( r2 * tabla[c] );
                    g1 += ( float )( g2 * tabla[c] );
                    b1 += ( float )( b2 * tabla[c] );
                }
            }
            r1 /= tabla[9];
            g1 /= tabla[9];
            b1 /= tabla[9];
            r = ((( int )r1 ) > 255 ) ? 255 : ( int )r1;
            g = ((( int )g1 ) > 255 ) ? 255 : ( int )g1;
            b = ((( int )b1 ) > 255 ) ? 255 : ( int )b1;

            if ( r < 0 )r = 0;
            if ( g < 0 )g = 0;
            if ( b < 0 )b = 0;

            if ( !r && !g && !b )
                c = 0;
            else
                c = gr_rgb( r, g, b );

            _put_pixel( map2, i, j, c );
            r1 = 0;
            g1 = 0;
            b1 = 0;
            c = 0;
        }
    }
    memcpy( map->data, map2->data, map->height*map->pitch );
    bitmap_destroy( map2 );

    return 1 ;
}

static int modeffects_blur( INSTANCE *my, int *params ) // fpg,map,tipo
{

    GRAPH * map = bitmap_get( params[0], params[1] ), *map2;

    int x, y, i, j, c;
    int r, g, b, r2, g2, b2;
    int color;

    if ( !map ) return 0;
    if ( map->format->depth != 16 ) return 0;

    switch ( params[2] )
    {
        case BLUR_NORMAL:
            //METODO 1 "RAPIDO" izq y arriba
            for ( i = 0;i < map->width;i++ )
                for ( j = 0;j < map->height;j++ )
                {
                    color = _get_pixel( map, i, j ) ;
                    if ( !color ) continue;
                    gr_get_rgb( color, &r, &g, &b );
                    if ( i > 0 )
                    {
                        gr_get_rgb( _get_pixel( map, i - 1, j ), &r2, &g2, &b2 );
                    }
                    else
                    {
                        gr_get_rgb( _get_pixel( map, i + 1, j ), &r2, &g2, &b2 );
                    }
                    r += r2;
                    g += g2;
                    b += b2;
                    if ( j > 0 )
                    {
                        gr_get_rgb( _get_pixel( map, i, j - 1 ), &r2, &g2, &b2 );
                    }
                    else
                    {
                        gr_get_rgb( _get_pixel( map, i, j + 1 ), &r2, &g2, &b2 );
                    }
                    r += r2;
                    g += g2;
                    b += b2;
                    r /= 3;
                    g /= 3;
                    b /= 3;
                    _put_pixel( map, i, j, gr_rgb( r, g, b ) );
                }
            break;

        case BLUR_3x3:
            // METODO2 LENTO 3x3
            r = 0;
            g = 0;
            b = 0;
            c = 0;
            for ( i = 0;i < map->width;i++ )
                for ( j = 0;j < map->height;j++ )
                {
                    color = _get_pixel( map, i, j ) ;
                    if ( !color ) continue;
                    gr_get_rgb( color, &r, &g, &b );
                    c++;
                    for ( x = i - 1;x < i + 2;x++ )
                    {
                        for ( y = j - 1;y < j + 2;y++ )
                        {
                            if ( x < 0 || x > map->width - 1 || y < 0 || y > map->height - 1 ) continue;
                            gr_get_rgb( _get_pixel( map, x, y ), &r2, &g2, &b2 );
                            r += r2;
                            g += g2;
                            b += b2;
                            c++;
                        }
                    }
                    if ( !c ) continue;
                    r /= c;
                    g /= c;
                    b /= c;
                    _put_pixel( map, i, j, gr_rgb( r, g, b ) );
                    r = 0;
                    g = 0;
                    b = 0;
                    c = 0;
                }
            break;

        case BLUR_5x5:
            // METODO3 aun mas LENTO 5x5
            r = 0;
            g = 0;
            b = 0;
            c = 0;
            for ( i = 0;i < map->width;i++ )
            {
                for ( j = 0;j < map->height;j++ )
                {
                    color = _get_pixel( map, i, j ) ;
                    if ( !color ) continue;
                    gr_get_rgb( color, &r, &g, &b );
                    c++;
                    for ( x = i - 2;x < i + 3;x++ )
                    {
                        for ( y = j - 2;y < j + 3;y++ )
                        {
                            if ( x < 0 || x > map->width - 1 || y < 0 || y > map->height - 1 ) continue;
                            gr_get_rgb( _get_pixel( map, x, y ), &r2, &g2, &b2 );
                            r += r2;
                            g += g2;
                            b += b2;
                            c++;
                        }
                    }
                    if ( !c ) continue;
                    r /= c;
                    g /= c;
                    b /= c;
                    _put_pixel( map, i, j, gr_rgb( r, g, b ) );
                    r = 0;
                    g = 0;
                    b = 0;
                    c = 0;
                }
            }
            break;

        case BLUR_5x5_MAP:
            // METODO4 5x5 mapa adicional
            map2 = bitmap_clone( map );
            r = 0;
            g = 0;
            b = 0;
            c = 0;
            for ( i = 0;i < map->width;i++ )
            {
                for ( j = 0;j < map->height;j++ )
                {
                    color = _get_pixel( map, i, j ) ;
                    if ( !color ) continue;
                    gr_get_rgb( color, &r, &g, &b );
                    c++;
                    for ( x = i - 2;x < i + 3;x++ )
                    {
                        for ( y = j - 2;y < j + 3;y++ )
                        {
                            if ( x < 0 || x > map->width - 1 || y < 0 || y > map->height - 1 ) continue;
                            gr_get_rgb( _get_pixel( map, x, y ), &r2, &g2, &b2 );
                            r += r2;
                            g += g2;
                            b += b2;
                            c++;
                        }
                    }
                    if ( !c ) continue;
                    r /= c;
                    g /= c;
                    b /= c;
                    _put_pixel( map2, i, j, gr_rgb( r, g, b ) );
                    r = 0;
                    g = 0;
                    b = 0;
                    c = 0;
                }
            }
            memcpy( map->data, map2->data, map->height*map->pitch );
            bitmap_destroy( map2 );
            break;
        default:
            break;

    }
    return 1 ;
}

static int modeffects_grayscale( INSTANCE *my, int *params ) //fpg,map,tipo
{
    GRAPH * map = bitmap_get( params[0], params[1] ) ;
    uint32_t i, j, c;
    int r, g, b;

    if ( !map ) return 0;
    if ( map->format->depth != 16 ) return 0;

    for ( i = 0;i < map->height;i++ )
        for ( j = 0;j < map->width;j++ )
        {
            gr_get_rgb( _get_pixel( map, j, i ), &r, &g, &b );
            if ( !r && !g && !b )continue;
            c = ( int )( 0.3 * r + 0.59 * g + 0.11 * b );
            switch ( params[2] )
            {
                case GSCALE_RGB: // RGB
                    c = gr_rgb( c, c, c );
                    break;

                case GSCALE_R: // R
                    c = gr_rgb( c, 0, 0 );
                    break;

                case GSCALE_G: // G
                    c = gr_rgb( 0, c, 0 );
                    break;

                case GSCALE_B: // B
                    c = gr_rgb( 0, 0, c );
                    break;

                case GSCALE_RG: // RG
                    c = gr_rgb( c, c, 0 );
                    break;

                case GSCALE_RB: // RB
                    c = gr_rgb( c, 0, c );
                    break;


                case GSCALE_GB: // GB
                    c = gr_rgb( 0, c, c );
                    break;

                case GSCALE_OFF:
                default:
                    c = gr_rgb( r, g, b );
            }
            _put_pixel( map, j, i, c );
        }

    return 1 ;
}

static int modeffects_rgbscale( INSTANCE *my, int *params ) //fpg, map, r, g, b
{
    GRAPH * map = bitmap_get( params[0], params[1] ) ;
    uint32_t i, j, c;
    int r, g, b;

    if ( !map ) return 0;
    if ( map->format->depth != 16 ) return 0;

    for ( i = 0;i < map->height;i++ )
        for ( j = 0;j < map->width;j++ )
        {
            gr_get_rgb( _get_pixel( map, j, i ), &r, &g, &b );
            if ( !r && !g && !b )continue;
            c = ( int )( 0.3 * r + 0.59 * g + 0.11 * b );
            c = gr_rgb(( int )( c**( float * )( &params[2] ) ),
                    ( int )( c**( float * )( &params[3] ) ),
                    ( int )( c**( float * )( &params[4] ) ) );
            _put_pixel( map, j, i, c );
        }

    return 1 ;
}

/* --------------------------------------------------------------------------- */

DLCONSTANT __bgdexport( mod_effects, constants_def )[] =
{
    { "BLUR_NORMAL" , TYPE_INT, BLUR_NORMAL     },
    { "BLUR_3x3"    , TYPE_INT, BLUR_3x3        },
    { "BLUR_5x5"    , TYPE_INT, BLUR_5x5        },
    { "BLUR_5x5_MAP", TYPE_INT, BLUR_5x5_MAP    },

    { "GSCALE_RGB"  , TYPE_INT, GSCALE_RGB      },
    { "GSCALE_R"    , TYPE_INT, GSCALE_R        },
    { "GSCALE_G"    , TYPE_INT, GSCALE_G        },
    { "GSCALE_B"    , TYPE_INT, GSCALE_B        },
    { "GSCALE_RG"   , TYPE_INT, GSCALE_RG       },
    { "GSCALE_RB"   , TYPE_INT, GSCALE_RB       },
    { "GSCALE_GB"   , TYPE_INT, GSCALE_GB       },
    { "GSCALE_OFF"  , TYPE_INT, GSCALE_OFF      },

    { NULL          , 0       , 0               }
} ;

/* ----------------------------------------------------------------- */
/* Declaracion de funciones                                          */

DLSYSFUNCS  __bgdexport( mod_effects, functions_exports )[] =
{
    { "GRAYSCALE"   , "IIB"   , TYPE_INT    , modeffects_grayscale },
    { "RGBSCALE"    , "IIFFF" , TYPE_INT    , modeffects_rgbscale  },
    { "BLUR"        , "IIB"   , TYPE_INT    , modeffects_blur      },
    { "FILTER"      , "IIP"   , TYPE_INT    , modeffects_filter    },
    { NULL          , NULL    , 0           , NULL                 }
};

/* ----------------------------------------------------------------- */

char * __bgdexport( mod_effects, modules_dependency )[] =
{
    "libgrbase",
    NULL
};

/* --------------------------------------------------------------------------- */
