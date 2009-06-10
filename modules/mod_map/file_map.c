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

static GRAPH * gr_read_map( file * fp )
{
    char header[8] ;
    unsigned short int w, h, c ;
    uint32_t y ;
    int bpp, code ;
    GRAPH * gr ;
    int st = 0;
    PALETTE * pal = NULL;

    /* Carga los datos de cabecera */
    file_read( fp, header, 8 ) ;

    if ( strcmp( header, M32_MAGIC ) == 0 ) bpp = 32 ;
    else if ( strcmp( header, M16_MAGIC ) == 0 ) bpp = 16 ;
    else if ( strcmp( header, MAP_MAGIC ) == 0 ) bpp = 8 ;
    else if ( strcmp( header, M01_MAGIC ) == 0 ) bpp = 1 ;
    else return NULL ;

    file_readUint16( fp, &w ) ;
    file_readUint16( fp, &h ) ;
    file_readSint32( fp, &code ) ;

    gr = bitmap_new( code, w, h, bpp ) ;
    if ( !gr ) return NULL ;

    file_read( fp, gr->name, 32 ) ;
    gr->name[31] = 0 ;

    /* Datos de paleta */

    if ( gr->format->depth == 8 )
        if ( !( pal = gr_read_pal_with_gamma( fp ) ) )
        {
            bitmap_destroy( gr );
            return NULL ;
        }

    /* Puntos de control */

    file_readUint16( fp, &c ) ;
    gr->ncpoints = c ;

    if ( gr->ncpoints )
    {
        gr->cpoints = ( CPOINT * ) malloc( c * sizeof( CPOINT ) ) ;
        if ( !gr->cpoints )
        {
            bitmap_destroy( gr );
            pal_destroy( pal );
            return NULL ;
        }

        for ( c = 0 ; c < gr->ncpoints ; c++ )
        {
            file_readUint16( fp, &w ) ;
            file_readUint16( fp, &h ) ;
            if (( short int ) w == -1 && ( short int ) h == -1 )
            {
                w = CPOINT_UNDEFINED;
                h = CPOINT_UNDEFINED;
            }
            gr->cpoints[c].x = w ;
            gr->cpoints[c].y = h ;
        }
    }
    else
        gr->cpoints = 0 ;

    /* Datos del gráfico */

    for ( y = 0 ; y < gr->height ; y++ )
    {
        uint8_t * line = ( uint8_t * )gr->data + gr->pitch * y;

        switch ( bpp )
        {
            case    32:
                st = file_readUint32A( fp, ( uint32_t * )line, gr->width );
                break;

            case    16:
                st = file_readUint16A( fp, ( uint16_t * )line, gr->width );
                break;

            case    8:
            case    1:
                st = file_read( fp, line, gr->widthb );
                break;
        }

        if ( !st )
        {
            bitmap_destroy( gr );
            pal_destroy( pal );
            return NULL ;
        }
    }

    gr->format->palette = pal;
    pal_use( pal );

    gr->modified = 0 ;
    bitmap_analize( gr );

    return gr ;
}

/* --------------------------------------------------------------------------- */

int gr_save_map( GRAPH * gr, char * filename )
{
    int c, st = 0;
    MAP_HEADER mh ;
    uint8_t gamma[576];
    uint16_t cpoints;

    if ( !gr ) return 0;

    file * fp = file_open( filename, "wb" );
    if ( !fp ) return 0;

    switch ( gr->format->depth )
    {
        case    8:
            strcpy( ( char * ) mh.magic, MAP_MAGIC );
            break;

        case    32:
            strcpy( ( char * ) mh.magic, M32_MAGIC );
            break;

        case    16:
            strcpy( ( char * ) mh.magic, M16_MAGIC );
            break;

        case    1:
            strcpy( ( char * ) mh.magic, M01_MAGIC );
            break;
    }

    mh.version = 0x00;
    mh.width = gr->width;
    mh.height = gr->height;
    mh.code = gr->code ;
    strncpy( ( char * ) mh.name, gr->name, 32 );

    file_write( fp, &mh, sizeof( MAP_HEADER ) ) ;

    if ( gr->format->depth == 8 )
    {
        if ( gr->format->palette )
        {
            file_write( fp, gr->format->palette->rgb, 768 ) ;
        }
        else if ( sys_pixel_format->palette )
        {
            file_write( fp, sys_pixel_format->palette->rgb, 768 ) ;
        }
        else
        {
            file_write( fp, default_palette, 768 ) ;
        }
        memset( gamma, '\0', sizeof( gamma ) );
        file_write( fp, gamma, sizeof( gamma ) ) ;
    }

    cpoints = gr->ncpoints;
    file_writeUint16( fp, &cpoints );

    for ( c = 0 ; c < gr->ncpoints ; c++ )
    {
        file_writeUint16( fp, ( uint16_t * ) &gr->cpoints[c].x ) ;
        file_writeUint16( fp, ( uint16_t * ) &gr->cpoints[c].y ) ;
    }

    for ( c = 0 ; c < gr->height ; c++ )
    {
        uint8_t * line = ( uint8_t * )gr->data + gr->pitch * c;

        switch ( gr->format->depth )
        {
            case    32:
                st = file_writeUint32A( fp, ( uint32_t * )line, gr->width );
                break;

            case    16:
                st = file_writeUint16A( fp, ( uint16_t * )line, gr->width );
                break;

            case    8:
            case    1:
                st = file_write( fp, line, gr->widthb );
                break;
        }

        if ( !st ) break;
    }

    file_close( fp ) ;

    return 1;
}

/* --------------------------------------------------------------------------- */

/* Funciones de carga de nivel superior */

int gr_load_map( const char * mapname )
{
    GRAPH * gr ;
    file * fp = file_open( mapname, "rb" ) ;
    if ( !fp ) return 0 ;

    gr = gr_read_map( fp ) ;
    file_close( fp ) ;

    if ( !gr ) return 0 ;

    // Don't matter the file code, we must force a new code...
    gr->code = bitmap_next_code() ;

    grlib_add_map( 0, gr ) ;
    return gr->code ;
}

/* --------------------------------------------------------------------------- */
