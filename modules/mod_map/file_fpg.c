/*
 *  Copyright � 2006-2008 SplinterGU (Fenix/Bennugd)
 *  Copyright � 2002-2006 Fenix Team (Fenix)
 *  Copyright � 1999-2002 Jos� Luis Cebri�n Pag�e (Fenix)
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

struct
{
    int     code ;
    int     regsize ;
    char    name[32] ;
    char    fpname[12] ;
    int     width ;
    int     height ;
    int     flags ;
} chunk ;

/* --------------------------------------------------------------------------- */

/* Static convenience function */
static int gr_read_lib( file * fp )
{
    char header[8] ;
    short int px, py ;
    int bpp, libid, code;
    uint32_t   y ;
    unsigned c;
    GRLIB * lib ;
    GRAPH * gr ;
    PALETTE * pal = NULL ;
    int st = 0;

    libid = grlib_new() ;
    if ( libid < 0 ) return 0 ;

    lib = grlib_get( libid );
    if ( !lib ) return 0 ;

    file_read( fp, header, 8 ) ;

    if ( strcmp( header, F32_MAGIC ) == 0 ) bpp = 32 ;
    else if ( strcmp( header, F16_MAGIC ) == 0 ) bpp = 16 ;
    else if ( strcmp( header, FPG_MAGIC ) == 0 ) bpp = 8 ;
    else if ( strcmp( header, F01_MAGIC ) == 0 ) bpp = 1 ;
    else return 0 ;

    if ( bpp == 8 && !( pal = gr_read_pal_with_gamma( fp ) ) ) return 0 ;

    while ( !file_eof( fp ) )
    {
        if ( !file_read( fp, &chunk, 64 ) ) break ;

        ARRANGE_DWORD( &chunk.code ) ;
        ARRANGE_DWORD( &chunk.regsize ) ;
        ARRANGE_DWORD( &chunk.width ) ;
        ARRANGE_DWORD( &chunk.height ) ;
        ARRANGE_DWORD( &chunk.flags ) ;

        /* Cabecera del gr�fico */

        gr = bitmap_new( chunk.code, chunk.width, chunk.height, bpp ) ;
        if ( !gr )
        {
            grlib_destroy( libid ) ;
            if ( bpp == 8 ) pal_destroy( pal ) ; // Elimino la instancia inicial
            return 0 ;
        }
        memcpy( gr->name, chunk.name, 32 ) ;
        gr->name[31] = 0 ;
        gr->ncpoints = chunk.flags ;
        gr->modified = 2 ;
        // bitmap_analize( gr );

        /* Puntos de control */

        if ( gr->ncpoints )
        {
            gr->cpoints = ( CPOINT * ) malloc( gr->ncpoints * sizeof( CPOINT ) ) ;
            if ( !gr->cpoints )
            {
                bitmap_destroy( gr ) ;
                grlib_destroy( libid ) ;
                if ( bpp == 8 ) pal_destroy( pal ) ;
                return 0 ;
            }
            for ( c = 0 ; c < gr->ncpoints ; c++ )
            {
                file_readSint16( fp, &px ) ;
                file_readSint16( fp, &py ) ;
                if ( px == -1 && py == -1 )
                {
                    gr->cpoints[c].x = CPOINT_UNDEFINED ;
                    gr->cpoints[c].y = CPOINT_UNDEFINED ;
                }
                else
                {
                    gr->cpoints[c].x = px ;
                    gr->cpoints[c].y = py ;
                }
            }
        }
        else gr->cpoints = 0 ;

        /* Datos del gr�fico */

        for ( y = 0 ; y < gr->height ; y++ )
        {
            uint8_t * line = ( uint8_t * )gr->data + gr->pitch * y;

            switch ( bpp )
            {
                case    32:
                    st = file_readUint32A( fp, ( uint32_t * ) line, gr->width );
                    break;

                case    16:
                    st = file_readUint16A( fp, ( uint16_t * ) line, gr->width );
                    break;

                case    8:
                case    1:
                    st = file_read( fp, line, gr->width );
                    break;
            }

            if ( !st )
            {
                bitmap_destroy( gr );
                grlib_destroy( libid ) ;
                if ( bpp == 8 ) pal_destroy( pal );
                return 0 ;
            }
        }

        code = grlib_add_map( libid, gr ) ;
        if ( bpp == 8 ) pal_map_assign( libid, code, pal ) ;
    }

    if ( bpp == 8 ) pal_destroy( pal ) ; // Elimino la instancia inicial

    return libid ;
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : gr_load_fpg
 *
 *  Load a FPG file
 *
 *  PARAMS :
 *  libname   Name of the file
 *
 *  RETURN VALUE :
 *      Internal ID of the library or -1 if invalid file
 *
 */

int gr_load_fpg( const char * libname )
{
    int libid ;
    file * fp = file_open( libname, "rb" ) ;
    if ( !fp ) return -1 ;
    libid = gr_read_lib( fp ) ;
    file_close( fp ) ;
    return libid ;
}

/* --------------------------------------------------------------------------- */

/*
 *  FUNCTION : gr_save_fpg
 *
 *  Save a FPG file
 *
 *  PARAMS :
 *  filename  Name of the file
 *
 *  RETURN VALUE :
 *      1 if succeded
 *      >= 0 error
 *
 */

int gr_save_fpg( int libid, const char * filename )
{
    file  * fp;
    GRLIB * lib;
    uint8_t * block = NULL;
    int     n, y, c;
    int     bpp;
    int     nmaps;
    uint8_t header[8];
    rgb_component * palette = NULL;

    /* Get the library and open the file */

    if ( !libid )
        lib = syslib ;
    else
        lib = grlib_get( libid );

    if ( !lib ) return 0;

    /* Guess the FPG depth */

    nmaps = lib->map_reserved < 1000 ? lib->map_reserved : 1000;

    for ( bpp = n = 0 ; n < nmaps ; n++ )
    {
        if ( lib->maps[n] )
        {
            if ( bpp && lib->maps[n]->format->depth != bpp ) return 0; /* save_fpg: don't allow save maps with differents bpp */
            bpp = lib->maps[n]->format->depth;
            if ( !palette && bpp == 8 && lib->maps[n]->format->palette ) palette = lib->maps[n]->format->palette->rgb;
        }
    }

    if ( bpp == 32 ) strcpy( header, F32_MAGIC );
    else if ( bpp == 16 ) strcpy( header, F16_MAGIC );
    else if ( bpp == 8 ) strcpy( header, FPG_MAGIC );
    else if ( bpp == 1 ) strcpy( header, F01_MAGIC );
    else return 0; /* No maps for save */

    /* Create fpg */

    fp = file_open( filename, "wb" );
    if ( !fp ) return 0;

    /* Write the header */

    header[7] = 0x00; /* Version */
    file_write( fp, header, 8 );

    /* Write the color palette */

    if ( bpp == 8 )
    {
        uint8_t colors[256][3] ;
        uint8_t gamma[576];

        if ( !palette )
            if ( sys_pixel_format->palette )
                palette = sys_pixel_format->palette->rgb;
            else
                palette = ( rgb_component * ) default_palette;

        for ( n = 0 ; n < 256 ; n++ )
        {
            colors[n][0] = palette[n].r >> 2 ;
            colors[n][1] = palette[n].g >> 2 ;
            colors[n][2] = palette[n].b >> 2 ;
        }

        file_write( fp, &colors, 768 ) ;
        memset( gamma, '\0', sizeof( gamma ) );
        file_write( fp, gamma, sizeof( gamma ) ) ;
    }

    /* Write each map */

    for ( n = 0 ; n < nmaps ; n++ )
    {
        GRAPH * gr = lib->maps[n];

        if ( gr )
        {
            /* Write the bitmap header */

            chunk.code = n;
            chunk.regsize = 0;
            chunk.width = gr->width;
            chunk.height = gr->height;
            chunk.flags = gr->ncpoints;
            chunk.fpname[0] = 0;
            strncpy( chunk.name,  gr->name,  sizeof( chunk.name ) );

            ARRANGE_DWORD( &chunk.code );
            ARRANGE_DWORD( &chunk.regsize );
            ARRANGE_DWORD( &chunk.width );
            ARRANGE_DWORD( &chunk.height );
            ARRANGE_DWORD( &chunk.flags );

            file_write( fp, &chunk, 64 );

            /* Write the control points */

            for ( c = 0 ; c < gr->ncpoints ; c++ )
            {
                file_writeSint16( fp, &gr->cpoints[c].x );
                file_writeSint16( fp, &gr->cpoints[c].y );
            }

            /* Write the bitmap pixel data */

            if ( bpp > 8 )
            {
                if ( !( block = malloc( gr->widthb ) ) )
                {
                    file_close( fp );
                    return 0; /* No memory */
                }
            }

            for ( y = 0 ; y < gr->height ; y++ )
            {
                if ( bpp > 8 )
                {
                    memcpy( block, ( uint8_t * )gr->data + gr->pitch*y, gr->widthb );
                    if ( bpp == 16 )
                    {
//                        gr_convert16_ScreenTo565(( uint16_t * )block, gr->width );
                        file_writeUint16A( fp, ( uint16_t * ) block, gr->width );
                    }
                    else
                        file_writeUint32A( fp, ( uint32_t * ) block, gr->width );
                }
                else
                {
                    file_write( fp, ( uint8_t * )gr->data + gr->pitch*y, gr->widthb );
                }
            }

            if ( bpp > 8 ) free( block );
        }
    }

    file_close( fp );
    return 1;
}

/* --------------------------------------------------------------------------- */