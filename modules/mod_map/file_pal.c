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

#include "mod_map.h"

/* --------------------------------------------------------------------------- */

static uint8_t colors[256*3] ;

/* --------------------------------------------------------------------------- */

PALETTE * gr_read_pal( file * fp )
{
    PALETTE * pal;
    int i;

    if ( !file_read( fp, colors, sizeof( colors ) ) ) return NULL ;

    for ( i = 0; i < 768; i++ ) colors[i] <<= 2;

    pal = pal_new_rgb(( uint8_t * )colors );
    pal_refresh( pal );

    if ( !sys_pixel_format->palette )
    {
        sys_pixel_format->palette = pal_new( pal );
        palette_changed = 1 ;
    }

    return pal ;
}

/* --------------------------------------------------------------------------- */

PALETTE * gr_read_pal_with_gamma( file * fp )
{
    PALETTE * pal = gr_read_pal( fp );

    /* Ignora definiciones de gama */
    if ( pal ) file_seek( fp, 576, SEEK_CUR ) ;

    return pal ;
}

/* --------------------------------------------------------------------------- */

/*
 *  FUNCTION : gr_save_pal
 *
 *  Saves palette to the given file
 *
 *  PARAMS :
 *      filename        Name of file to create
 *      pal             Palette
 *
 *  RETURN VALUE :
 *      1 if succeded, 0 if error
 */

int gr_save_pal( const char * filename, PALETTE * pal )
{
    file * fp ;
    char header[8] = PAL_MAGIC;
    int i;

    if ( !pal ) return 0;

    fp = file_open( filename, "wb" );
    if ( !fp ) return 0;

    memmove( colors, pal->rgb, sizeof( colors ) );
    for ( i = 0; i < sizeof( colors ); i++ ) colors[i] >>= 2;

    header[7] = 0x00; /* Version */
    file_write( fp, header, 8 );
    file_write( fp, colors, 768 );

    memset( colors, 0, 576 );
    file_write( fp, colors, 576 );
    file_close( fp );

    return 1;
}

/* --------------------------------------------------------------------------- */

/*
 *  FUNCTION : gr_save_system_pal
 *
 *  Saves the current palette to the given file
 *
 *  PARAMS :
 *      filename        Name of file to create
 *
 *  RETURN VALUE :
 *      1 if succeded, 0 if error
 */

int gr_save_system_pal( const char * filename )
{
    file * fp = file_open( filename, "wb" );
    char header[8] = PAL_MAGIC;
    int i;

    if ( !fp ) return 0;

    memmove( colors, sys_pixel_format->palette ? ( uint8_t * ) sys_pixel_format->palette->rgb : default_palette, sizeof( colors ) );
    for ( i = 0; i < sizeof( colors ); i++ ) colors[i] >>= 2;

    header[7] = 0x00; /* Version */
    file_write( fp, header, 8 );
    file_write( fp, colors, 768 );

    memset( colors, 0, 576 );
    file_write( fp, colors, 576 );
    file_close( fp );

    return 1;
}

/* --------------------------------------------------------------------------- */

int gr_load_pal( const char * filename )
{
    file * fp = file_open( filename, "rb" ) ;
    char header[8] ;
    PALETTE * r = NULL ;
    int old_sys_pal = sys_pixel_format->palette ;

    if ( !fp ) return 0 ;

    file_read( fp, header, 8 ) ;
    if ( strcmp( header, MAP_MAGIC ) == 0 )
    {
        file_seek( fp, 48, SEEK_SET ) ;
        r = gr_read_pal_with_gamma( fp ) ;
    }
    else if (
        strcmp( header, FPG_MAGIC ) == 0 ||
        strcmp( header, FNT_MAGIC ) == 0 ||
        strcmp( header, PAL_MAGIC ) == 0 )
    {
        r = gr_read_pal_with_gamma( fp ) ;
    }
    else if ( memcmp( header, "\x89PNG", 4 ) == 0 )
    {
        GRAPH * graph ;
        file_close( fp );
        fp = NULL;
        graph = gr_read_png( filename );
        if ( graph )
        {
            r = graph->format->palette;
            pal_use( graph->format->palette );
            bitmap_destroy( graph );
        }
    }

    if ( r )
    {
        if ( old_sys_pal ) pal_destroy( sys_pixel_format->palette );

        sys_pixel_format->palette = pal_new( r );
        palette_changed = 1 ;
    }

    if ( fp ) file_close( fp ) ;
    return ( int ) r;
}

/* --------------------------------------------------------------------------- */
