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

#include <png.h>

#include "mod_map.h"

/* --------------------------------------------------------------------------- */

/*
 *  GLOBAL VARIABLES
 */

/* --------------------------------------------------------------------------- */

static uint8_t colors[256 * 3] ;

static file * png ;

/* --------------------------------------------------------------------------- */

static void user_read_data( png_structp png_ptr, png_bytep data, png_size_t length )
{
    file_read( png, data, length ) ;
}

GRAPH * gr_read_png( const char * filename )
{
    GRAPH * bitmap ;
    unsigned int n, x ;
    uint16_t * ptr ;
    uint32_t * ptr32 ;
    uint32_t * orig ;
    uint32_t * row ;
    uint32_t Rshift, Gshift, Bshift ;
    uint32_t Rmask, Gmask, Bmask ;

    png_bytep * rowpointers ;

    png_structp png_ptr ;
    png_infop info_ptr, end_info ;
    png_uint_32 width, height, rowbytes;
    int depth, color ;

    /* Abre el fichero y se asegura de que screen está inicializada */

    png = file_open( filename, "rb" ) ;
    if ( !png ) return NULL;

    /* Prepara las estructuras internas */

    png_ptr = png_create_read_struct( PNG_LIBPNG_VER_STRING, 0, 0, 0 ) ;
    if ( !png_ptr )
    {
        file_close( png );
        return NULL;
    }

    info_ptr = png_create_info_struct( png_ptr ) ;
    end_info = png_create_info_struct( png_ptr ) ;
    if ( !info_ptr || !end_info )
    {
        png_destroy_read_struct( &png_ptr, &info_ptr, &end_info ) ;
        file_close( png ) ;
        return NULL;
    }

    /* Rutina de error */

    if ( setjmp( png_ptr->jmpbuf ) )
    {
        png_destroy_read_struct( &png_ptr, &info_ptr, &end_info ) ;
        file_close( png ) ;
        return NULL;
    }

    /* Recupera información sobre el PNG */

    png_set_read_fn( png_ptr, 0, user_read_data ) ;
    png_read_info( png_ptr, info_ptr ) ;
    png_get_IHDR( png_ptr, info_ptr, &width, &height, &depth, &color, 0, 0, 0 ) ;

    row = malloc( sizeof( uint32_t ) * width );
    if ( !row )
    {
        png_destroy_read_struct( &png_ptr, &info_ptr, &end_info ) ;
        file_close( png ) ;
        return NULL;
    }

    rowpointers = malloc( sizeof( png_bytep ) * height );
    if ( !rowpointers )
    {
        png_destroy_read_struct( &png_ptr, &info_ptr, &end_info ) ;
        free( row ) ;
        file_close( png ) ;
        return NULL;
    }

    /* Configura los distintos modos disponibles */

    if ( color == PNG_COLOR_TYPE_GRAY /*|| color == PNG_COLOR_TYPE_GRAY_ALPHA*/ )
    {
        png_set_gray_to_rgb( png_ptr );
        if ( color == PNG_COLOR_TYPE_GRAY ) png_set_filler( png_ptr, 0xFF, PNG_FILLER_AFTER ) ;
    }

    if ( depth == 16 ) png_set_strip_16( png_ptr ) ;

    if ( color == PNG_COLOR_TYPE_RGB ) png_set_filler( png_ptr, 0xFF, PNG_FILLER_AFTER ) ;

    png_set_bgr( png_ptr ) ;

    /* Recupera el fichero, convirtiendo a 16 bits si es preciso */

    rowbytes = png_get_rowbytes( png_ptr, info_ptr ) ;
    bitmap = bitmap_new( 0, width, height, ( color == PNG_COLOR_TYPE_PALETTE ) ? 8 : ( sys_pixel_format->depth == 16 ? 16 : 32 ) ) ;
    if ( !bitmap )
    {
        png_destroy_read_struct( &png_ptr, &info_ptr, &end_info ) ;
        free( rowpointers ) ;
        free( row ) ;
        file_close( png ) ;
        return NULL;
    }

    if ( color == PNG_COLOR_TYPE_PALETTE )
    {
        /* Read the color palette */

        png_colorp png_palette = ( png_colorp ) png_malloc( png_ptr, 256 * sizeof( png_color ) ) ;
        if ( !png_palette )
        {
            png_destroy_read_struct( &png_ptr, &info_ptr, &end_info ) ;
            free( rowpointers ) ;
            free( row ) ;
            file_close( png ) ;
            return NULL;
        }
        png_get_PLTE( png_ptr, info_ptr, &png_palette, ( int * ) &n ) ;

        uint8_t * p = colors;

        for ( n = 0; n < 256 ; n++ )
        {
            * p++ = png_palette[n].red;
            * p++ = png_palette[n].green;
            * p++ = png_palette[n].blue;
        }

        bitmap->format->palette = pal_new_rgb(( uint8_t * )colors );
        pal_refresh( bitmap->format->palette );

        if ( !sys_pixel_format->palette )
        {
            sys_pixel_format->palette = pal_new( bitmap->format->palette );
            pal_use( bitmap->format->palette );
            palette_changed = 1 ;
        }

        for ( n = 0 ; n < height ; n++ ) rowpointers[n] = (( uint8_t* )bitmap->data ) + n * bitmap->pitch ;
        png_read_image( png_ptr, rowpointers ) ;

        /* If the depth is less than 8, expand the pixel values */

        if ( depth == 4 )
        {
            for ( n =  0; n < height; n++ )
            {
                char * orig, * dest;
                orig = ( char *  ) ( rowpointers[n] );
                dest = orig + width - 1;
                orig += ( width - 1 ) / 2;

                for ( x = width; x--; )
                {
                    *dest-- = ( *orig >> ((( 1 - ( x & 0x01 ) ) << 2 ) ) ) & 0x0F ;
                    if ( !( x & 0x01 ) ) orig--;
                }
            }
        }
        else if ( depth == 2 )
        {
            for ( n = 0; n < height; n++ )
            {
                char * orig, * dest;
                orig = ( char * ) rowpointers[n];
                dest = orig + width - 1;
                orig += ( width - 1 ) / 4;

                for ( x = width; x--; )
                {
                    *dest-- = ( *orig >> ((( 3 - ( x & 0x03 ) ) << 1 ) ) ) & 0x03 ;
                    if ( !( x & 0x03 ) ) orig--;
                }
            }
        }
        else if ( depth == 1 )
        {
            for ( n = 0; n < height; n++ )
            {
                char * orig, * dest;
                orig = ( char * ) rowpointers[n];
                dest = orig + width - 1;
                orig += ( width - 1 ) / 8;

                for ( x = width; x--; )
                {
                    *dest-- = ( *orig >> ( 7 - ( x & 0x07 ) ) ) & 0x01 ;
                    if ( !( x & 0x07 ) ) orig--;
                }
            }
        }
    }
    else if ( depth == 8 && sys_pixel_format->depth != 16 )
    {
        for ( n = 0 ; n < height ; n++ )
        {
            rowpointers[0] = ( void * )row ;
            png_read_rows( png_ptr, rowpointers, 0, 1 ) ;

            ptr32 = ( uint32_t* )((( uint8_t * )bitmap->data ) + n * bitmap->pitch );
            orig = row ;
            for ( x = 0 ; x < width ; x++ )
            {
                ARRANGE_DWORD( orig );
                *ptr32 = *orig ;
                ptr32++, orig++ ;
            }
        }
    }
    else
    {
        Rshift = 8;
        Gshift = 5;
        Bshift = 3;

        Rmask = 0xF80000 ; // 3
        Gmask = 0x00FC00 ; // 2
        Bmask = 0x0000F8 ; // 3

        for ( n = 0 ; n < height ; n++ )
        {
            rowpointers[0] = ( void * )row ;
            png_read_rows( png_ptr, rowpointers, 0, 1 ) ;

            ptr = ( uint16_t* )((( uint8_t * )bitmap->data ) + n * bitmap->pitch );
            orig = row ;
            for ( x = 0 ; x < width ; x++ )
            {
                ARRANGE_DWORD( orig );

//                if (( *orig ) & 0xff000000 )
                if (( *orig ) & 0x80000000 )
                {
                    *ptr = (( *orig & Rmask ) >> Rshift ) | (( *orig & Gmask ) >> Gshift ) | (( *orig & Bmask ) >> Bshift )  ;
                    if ( !*ptr )( *ptr )++ ;
                }
                else
                    *ptr = 0 ;
                ptr++, orig++ ;
            }
        }
    }

    /* Fin */

    if ( !setjmp( png_ptr->jmpbuf ) ) png_read_end( png_ptr, 0 ) ;

    bitmap->modified = 1 ;

    png_destroy_read_struct( &png_ptr, &info_ptr, &end_info ) ;
    free( rowpointers ) ;
    free( row ) ;
    file_close( png ) ;

    return bitmap ;
}

/*
 *  FUNCTION : gr_save_png
 *
 *  Save a GRAPH into a PNG file
 *
 *  PARAMS :
 *      gr              GRAPH with the image to save
 *      filename        name for the file
 *
 *  RETURN VALUE :
 *      0 - FAILURE
 *      1 - SUCCESS
 *
 */

int gr_save_png( GRAPH * gr, const char * filename )
{
    if ( !gr ) return( 0 ) ;

    FILE * file = fopen( filename, "wb" ) ;
    png_structp png_ptr ;
    png_infop info_ptr ;
    png_uint_32 k, i ;
    png_bytep * rowpointers ;
    png_colorp pal ;
    uint32_t * data, * ptr ;
    uint16_t * orig ;
    uint32_t * orig32 ;
    rgb_component * gpal = NULL;

    if ( !file ) return( 0 ) ;

    rowpointers = malloc( sizeof( png_bytep ) * gr->height );
    if ( !rowpointers )
    {
        fclose( file ) ;
        return 0 ;
    }

    png_ptr  = png_create_write_struct( PNG_LIBPNG_VER_STRING, 0, 0, 0 ) ;
    if ( !png_ptr )
    {
        free( rowpointers ) ;
        fclose( file ) ;
        return( 0 ) ;
    }

    info_ptr = png_create_info_struct( png_ptr ) ;
    if ( !info_ptr )
    {
        png_destroy_write_struct( &png_ptr, NULL ) ;
        free( rowpointers ) ;
        fclose( file ) ;
        return( 0 ) ;
    }

    /* Error handling... */

    if ( setjmp( png_ptr->jmpbuf ) )
    {
        png_destroy_write_struct( &png_ptr, NULL ) ;
        free( rowpointers ) ;
        fclose( file ) ;
        return( 0 ) ;
    }

    png_init_io( png_ptr, file ) ;

    if ( gr->format->depth == 8 )
    {
        /* 8 bits PNG file */
        png_set_IHDR( png_ptr, info_ptr, gr->width,
                gr->height, 8, PNG_COLOR_TYPE_PALETTE,
                PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE,
                PNG_FILTER_TYPE_BASE ) ;

        pal = ( png_colorp ) png_malloc( png_ptr, 256 * sizeof( png_color ) ) ;
        if ( !pal )
        {
            png_destroy_write_struct( &png_ptr, NULL ) ;
            free( rowpointers ) ;
            fclose( file ) ;
            return( 0 ) ;
        }

        if ( gr->format->palette )
            gpal = gr->format->palette->rgb;
        else if ( sys_pixel_format->palette )
            gpal = sys_pixel_format->palette->rgb;
        else
            gpal = ( rgb_component * )default_palette;

        /* Generate palette info */
        for ( k = 0 ; k < 256 ; k++ )
        {
            pal[k].red   = gpal[k].r ;
            pal[k].green = gpal[k].g ;
            pal[k].blue  = gpal[k].b ;
        }
        png_set_PLTE( png_ptr, info_ptr, pal, 256 ) ;
        png_write_info( png_ptr, info_ptr ) ;

        /* no need to rearrange data */
        for ( k = 0 ; k < ( unsigned )gr->height ; k++ )
            rowpointers[k] = ( uint8_t * )gr->data + gr->pitch * k ;
        png_write_image( png_ptr, rowpointers ) ;

        /* Free allocated palette... */
        png_free( png_ptr, ( png_voidp ) pal ) ;
        info_ptr->palette = NULL ;
    }
    else
    {
        png_set_IHDR( png_ptr, info_ptr, gr->width,
                gr->height, 8, PNG_COLOR_TYPE_RGB_ALPHA,
                PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE,
                PNG_FILTER_TYPE_BASE ) ;
        png_write_info( png_ptr, info_ptr ) ;

        data = malloc( gr->width * gr->height * 4 ) ;
        if ( !data )
        {
            png_destroy_write_struct( &png_ptr, NULL ) ;
            free( rowpointers ) ;
            fclose( file ) ;
            return( 0 ) ;
        }

        if ( gr->format->depth == 16 )
        {
            for ( k = 0; k < ( unsigned )gr->height; k++ )
            {
                ptr  = data + gr->width * k ; /* uses dword for each pixel! */
                orig = ( uint16_t * )( gr->data + gr->pitch * k ) ;
                rowpointers[k] = ( uint8_t * )ptr ;
                for ( i = 0 ; i < ( unsigned )gr->width ; i++ )
                {
                    if ( *orig == 0 && !( gr->info_flags & GI_NOCOLORKEY ) )
                        *ptr = 0x00000000 ;
                    else
                    {
                        *ptr =
                            (( *orig & 0xf800 ) >> 8 ) |
                            (( *orig & 0x07e0 ) << 5 ) |
                            (( *orig & 0x001f ) << 19 ) |
                            0xFF000000 ;
                        /* Rearrange data */
                        ARRANGE_DWORD( ptr ) ;
                    }
                    orig++ ;
                    ptr++  ;
                }
            }
        }
        else if ( gr->format->depth == 32 )
        {
            for ( k = 0; k < ( unsigned )gr->height; k++ )
            {
                ptr  = data + gr->width * k ; /* uses dword for each pixel! */
                orig32 = ( uint32_t * )( gr->data + gr->pitch * k ) ;
                rowpointers[k] = ( uint8_t * )ptr ;
                for ( i = 0 ; i < ( unsigned )gr->width ; i++ )
                {
                    if ( !*orig32 && !( gr->info_flags & GI_NOCOLORKEY ) )
                        *ptr = 0 ;
                    else if ( !( *orig32 & 0xff000000 ) )
                        *ptr =
                            0xff000000 |
                            (( *orig32 & 0x00ff0000 ) >> 16 ) |
                            (( *orig32 & 0x0000ff00 ) ) |
                            (( *orig32 & 0x000000ff ) << 16 ) ;
                    else
                        *ptr =
                            (( *orig32 & 0xff000000 ) ) |
                            (( *orig32 & 0x00ff0000 ) >> 16 ) |
                            (( *orig32 & 0x0000ff00 ) ) |
                            (( *orig32 & 0x000000ff ) << 16 ) ;

                    /* Rearrange data */
                    ARRANGE_DWORD( ptr ) ;

                    orig32++ ;
                    ptr++  ;
                }
            }
        }
        png_write_image( png_ptr, rowpointers ) ;
        free( data ) ;
    }

    png_write_end( png_ptr, info_ptr ) ;
    fclose( file ) ;
    png_destroy_write_struct( &png_ptr, NULL ) ;
    free( rowpointers ) ;
    return ( 1 ) ;
}

/* --------------------------------------------------------------------------- */

int gr_load_png( const char * mapname )
{
    GRAPH * gr = gr_read_png( mapname ) ;
    if ( !gr ) return 0 ;
    gr->code = bitmap_next_code() ;
    grlib_add_map( 0, gr ) ;
    return gr->code ;
}

/* --------------------------------------------------------------------------- */
