/*
 *  Copyright © 2006-2008 SplinterGU (Fenix/Bennugd)
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

#include "libfont.h"
#include "mod_map.h"

/* --------------------------------------------------------------------------- */

static int gr_font_loadfrom( file * fp );

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : gr_font_load
 *
 *  Load a font from a given file, in FNT (DIV) format
 *
 *  PARAMS :
 *  filename  Name of the file
 *
 *  RETURN VALUE :
 *      ID of the new font, or -1 if error
 *
 */

int gr_font_load( char * filename )
{
    file * fp ;
    int result;

    fp = file_open( filename, "rb" ) ;
    if ( !fp ) return -1 ;
    result = gr_font_loadfrom( fp );
    file_close( fp );
    return result;
}

/* --------------------------------------------------------------------------- */

static int gr_font_loadfrom( file * fp )
{
    char header[8];
    int bpp;
    int types, i, id;
    Uint32 y;
    FONT * f;
    PALETTE * pal = NULL;

    struct
    {
        int width ;
        int height ;
        int yoffset ;
        int fileoffset ;
    }
    oldchardata[256];

    struct
    {
        int width;
        int height;
        int xadvance;
        int yadvance;
        int xoffset;
        int yoffset;
        int fileoffset ;
    }
    chardata[256] ;

    if ( font_count == MAX_FONTS ) return -1 ;

    /* Read the file header */

    if ( !file_read( fp, header, 8 ) ) return -1;

    if (
        memcmp( header, FNT_MAGIC, 7 ) != 0 &&
        memcmp( header, FNX_MAGIC, 7 ) != 0 )
    {
        return -1;
    }

    bpp = header[7];
    if ( bpp == 0 ) bpp = 8;

    /* Read or ignore the palette */

    if ( bpp == 8 && !( pal = gr_read_pal_with_gamma( fp ) ) ) return -1 ;

    /* Read the character data (detect old format) */

    if ( header[2] == 'x' )
    {
        if ( !file_readSint32( fp, &types ) )
        {
            pal_destroy( pal );
            return -1 ;
        }
        if ( !file_read( fp, chardata, sizeof( chardata ) ) )
        {
            pal_destroy( pal );
            return -1 ;
        }
        for ( i = 0 ; i < 256 ; i++ )
        {
            ARRANGE_DWORD( &chardata[i].width );
            ARRANGE_DWORD( &chardata[i].height );
            ARRANGE_DWORD( &chardata[i].xadvance );
            ARRANGE_DWORD( &chardata[i].yadvance );
            ARRANGE_DWORD( &chardata[i].xoffset );
            ARRANGE_DWORD( &chardata[i].yoffset );
            ARRANGE_DWORD( &chardata[i].fileoffset );
        }
    }
    else
    {
        if ( !file_readSint32( fp, &types ) )
        {
            pal_destroy( pal );
            return -1 ;
        }
        if ( !file_read( fp, oldchardata, sizeof( oldchardata ) ) )
        {
            pal_destroy( pal );
            return -1 ;
        }
        for ( i = 0 ; i < 256 ; i++ )
        {
            ARRANGE_DWORD( &oldchardata[i].width );
            ARRANGE_DWORD( &oldchardata[i].height );
            ARRANGE_DWORD( &oldchardata[i].yoffset );
            ARRANGE_DWORD( &oldchardata[i].fileoffset );

            chardata[i].width      = oldchardata[i].width;
            chardata[i].height     = oldchardata[i].height;
            chardata[i].xoffset    = 0;
            chardata[i].yoffset    = oldchardata[i].yoffset;
            chardata[i].xadvance   = oldchardata[i].width;
            chardata[i].yadvance   = oldchardata[i].height + oldchardata[i].yoffset;
            chardata[i].fileoffset = oldchardata[i].fileoffset;
        }
    }

    /* Create the font */

    id = gr_font_new() ;
    if ( id == -1 )
    {
        pal_destroy( pal );
        return -1 ;
    }

    f = fonts[id];
    if ( !f )
    {
        gr_font_destroy( id );
        pal_destroy( pal );
        return -1 ;
    }

    if ( header[2] == 'x' )
    {
        f->bpp = header[7];
        f->charset = types;
    }
    else
    {
        f->bpp = 8;
        f->charset = CHARSET_CP850;
    }

    /* Load the character bitmaps */

    for ( i = 0 ; i < 256 ; i++ )
    {
        GRAPH * gr;
        Uint8 * ptr;

        f->glyph[i].xadvance = chardata[i].xadvance ;
        f->glyph[i].yadvance = chardata[i].yadvance ;

        if ( chardata[i].fileoffset == 0 || chardata[i].width == 0 || chardata[i].height == 0 ) continue ;

        f->glyph[i].xoffset = chardata[i].xoffset ;
        f->glyph[i].yoffset = chardata[i].yoffset ;

        file_seek( fp, chardata[i].fileoffset, SEEK_SET ) ;
        f->glyph[i].bitmap = gr = bitmap_new( i, chardata[i].width, chardata[i].height, f->bpp ) ;
        if ( !gr )
        {
            gr_font_destroy( id );
            pal_destroy( pal );
            return -1 ;
        }
        bitmap_add_cpoint( gr, 0, 0 ) ;
        gr->format->palette = pal;
        pal_use( pal );

        for ( y = 0, ptr = gr->data ; y < gr->height ; y++, ptr += gr->pitch )
        {
            if ( !file_read( fp, ptr, gr->widthb ) ) break ;

            if ( gr->format->depth == 16 )
            {
                gr_convert16_565ToScreen(( Uint16 * )ptr, gr->width );
                ARRANGE_WORDS( ptr, ( int )gr->width );
            }
            else if ( gr->format->depth == 32 )
            {
                ARRANGE_DWORDS( ptr, ( int )gr->width );
            }
        }

        f->glyph[i].yoffset = chardata[i].yoffset ;
    }
    if ( f->glyph[32].xadvance == 0 ) f->glyph[32].xadvance = 4 ;

    pal_destroy( pal ); // Elimino la instancia inicial

    return id ;
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : gr_font_save
 *
 *  Write a font to disk, in FNT/FNX format
 *
 *  PARAMS :
 *  fontid   ID of the font to save
 *  filename  Name of the file to create
 *
 *  RETURN VALUE :
 *      1 if succeded or 0 otherwise
 *
 */

int gr_font_save( int fontid, const char * filename )
{
    gzFile * file;
    int      n;
    Uint32   y;
    long     offset;
    Uint8 *  block = NULL ;
    Uint8 *  lineptr;

    FONT *   font;
    Uint8    header[8];
    struct
    {
        int width;
        int height;
        int xadvance;
        int yadvance;
        int xoffset;
        int yoffset;
        int fileoffset ;
    }
    chardata[256] ;
    int palette_saved = 0;

    if ( fontid < 0 || fontid > MAX_FONTS || !fonts[fontid] ) return 0;

    font = fonts[fontid];

    /* Open the file */

    file = gzopen( filename, "wb" );
    if ( !file ) return 0;

    /* Write the header */

    strcpy( header, FNX_MAGIC );
    header[7] = font->bpp;
    gzwrite( file, &header, 8 );

    /* Write the character information */

    memset( chardata, 0, sizeof( chardata ) );
    offset = 8 + 4 + (( font->bpp == 8 ) ? 576 + 768 : 0 ) + sizeof( chardata );

    for ( n = 0 ; n < 256 ; n++ )
    {
        chardata[n].xadvance   = font->glyph[n].xadvance;
        chardata[n].yadvance   = font->glyph[n].yadvance;

        if ( font->glyph[n].bitmap )
        {
            /* Write the palette */
            if ( !palette_saved && font->bpp == 8 )
            {
                Uint8   colors[256][3];
                Uint8 * block = calloc( 576, 1 ) ;
                rgb_component * gpal = NULL;
                int k;

                if ( font->glyph[n].bitmap->format->palette )
                    gpal = font->glyph[n].bitmap->format->palette->rgb;
                else if ( sys_pixel_format->palette )
                    gpal = sys_pixel_format->palette->rgb;
                else
                    gpal = ( rgb_component * ) default_palette;

                /* Generate palette info */
                for ( k = 0 ; k < 256 ; k++ )
                {
                    colors[k][0] = gpal[k].r >> 2 ;
                    colors[k][1] = gpal[k].g >> 2 ;
                    colors[k][2] = gpal[k].b >> 2 ;
                }

                gzwrite( file, &colors, 768 ) ;
                gzwrite( file, block, 576 ) ;
                free( block ) ;
                palette_saved = 1;
            }

            chardata[n].width      = font->glyph[n].bitmap->width;
            chardata[n].height     = font->glyph[n].bitmap->height;
            chardata[n].xadvance   = font->glyph[n].xadvance;
            chardata[n].yadvance   = font->glyph[n].yadvance;
            chardata[n].xoffset    = font->glyph[n].xoffset;
            chardata[n].yoffset    = font->glyph[n].yoffset;
            chardata[n].fileoffset = offset;

            offset += font->glyph[n].bitmap->widthb * chardata[n].height;
        }

        ARRANGE_DWORD( &chardata[n].xadvance );
        ARRANGE_DWORD( &chardata[n].yadvance );
        ARRANGE_DWORD( &chardata[n].width );
        ARRANGE_DWORD( &chardata[n].width );
        ARRANGE_DWORD( &chardata[n].xoffset );
        ARRANGE_DWORD( &chardata[n].yoffset );
        ARRANGE_DWORD( &chardata[n].fileoffset );
    }

    ARRANGE_DWORD( &font->charset );
    gzwrite( file, &font->charset, 4 );
    ARRANGE_DWORD( &font->charset );

    gzwrite( file, &chardata, sizeof( chardata ) );

    /* Write the character bitmaps */

    for ( n = 0 ; n < 256 ; n++ )
    {
        if ( font->glyph[n].bitmap )
        {
            GRAPH * gr = font->glyph[n].bitmap;

            if ( gr->format->depth != font->bpp )
            {
                gzclose( file );
                return 0;
            }

            if ( gr->format->depth > 8 )
            {
                if (( block = malloc( gr->widthb ) ) == NULL )
                {
                    gzclose( file );
                    return 0;
                }
            }

            lineptr = gr->data;

            for ( y = 0 ; y < gr->height ; y++, lineptr += gr->pitch )
            {
                if ( gr->format->depth > 8 )
                {
                    memcpy( block, lineptr, gr->widthb );
                    if ( gr->format->depth == 16 )
                    {
                        ARRANGE_WORDS( block, ( int )gr->width );
                        gr_convert16_ScreenTo565(( Uint16 * )block, gr->width );
                    }
                    else if ( gr->format->depth == 32 )
                    {
                        ARRANGE_DWORDS( block, ( int )gr->width );
                    }
                    gzwrite( file, block, gr->widthb );
                }
                else
                {
                    gzwrite( file, lineptr, gr->widthb );
                }
            }

            if ( gr->format->depth > 8 ) free( block );
        }
    }

    gzclose( file );

    return 1;
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : gr_load_bdf
 *
 *  Load a BDF font from disk. This is a very simple loader that ignores
 *  anything that is not relevant to screen display or non-horizontal
 *  writing fonts.
 *
 *  PARAMS :
 *  filename  Name of the BDF file
 *
 *  RETURN VALUE :
 *      ID of the font if succeded or -1 otherwise
 *
 */

int gr_load_bdf( const char * filename )
{
    file * fp;
    Uint8 line[2048];
    Uint8 * ptr;
    Uint8 * optr;
    FONT * font;
    int id, x, y, i;
    int error = 0;

    Uint8 nibbleh[256];
    Uint8 nibblel[256];

    int default_xadvance = 0;
    int default_yadvance = 0;
    int in_char = 0;
    int encoding = -1;
    int width = 0;
    int height = 0;
    int xoffset = 0;
    int yoffset = 0;
    int xadvance = 0;
    int yadvance = 0;
    int minyoffset = 0;
    int len;

    /* Arrays used to convert hex ASCII to binary */

    memset( nibbleh, 0, 256 );
    memset( nibblel, 0, 256 );

    for ( i = '0' ; i <= '9' ; i++ )
    {
        nibbleh[i] = (( i - '0' ) << 4 );
        nibblel[i] = i - '0';
    }
    for ( i = 10 ; i <= 15 ; i++ )
    {
        nibbleh['A' + i - 10] = ( i << 4 );
        nibbleh['a' + i - 10] = ( i << 4 );
        nibblel['A' + i - 10] = i;
        nibblel['a' + i - 10] = i;
    }

    /* Open the file and create the font */

    fp = file_open( filename, "r" );
    if ( !fp ) return -1;

    id = gr_font_new();
    if ( id < 0 ) return -1;
    font = fonts[id];
    font->bpp = 1;
    font->charset = CHARSET_ISO8859;
    font->maxwidth = 0;
    font->maxheight = 0;

    /* Process the file, a line each time */

    for ( line[2047] = 0 ; ; )
    {
        if ( !( len = file_gets( fp, line, 2047 ) ) ) break;
        if ( line[len-1] == '\n' ) line[len-1] = '\0';

        /* Handle global-level commands */

        if ( strncmp( line, "DWIDTH ", 7 ) == 0 && !in_char )
        {
            default_xadvance = atoi( line + 7 );
            ptr = strchr( line + 7, ' ' );
            if ( ptr ) default_yadvance = atoi( ptr + 1 );
        }
        else if ( strncmp( line, "STARTCHAR", 9 ) == 0 )
        {
            in_char = 1;
            encoding = -1;
            height = 0;
            xadvance = default_xadvance;
            yadvance = default_yadvance;
        }
        else if ( strncmp( line, "ENDCHAR", 7 ) == 0 )
        {
            in_char = 0;
        }

        /* Handle character-level commands */

        else if ( strncmp( line, "DWIDTH ", 7 ) == 0 && in_char )
        {
            xadvance = atoi( line + 7 );
            ptr = strchr( line + 7, ' ' );
            if ( ptr ) yadvance = atoi( ptr + 1 );
        }
        else if ( strncmp( line, "ENCODING ", 9 ) == 0 && in_char )
        {
            encoding = atoi( line + 9 );
            if ( encoding == -1 )
            {
                ptr = strchr( line + 7, ' ' );
                if ( ptr ) encoding = atoi( ptr + 1 );
            }
        }
        else if ( strncmp( line, "BBX ", 4 ) == 0 && in_char )
        {
            width = atoi( line + 4 );
            if ( width & 7 ) width = ( width & ~7 ) + 8;
            if (( ptr = strchr( line + 4, ' ' ) ) == NULL ) continue;
            height = atoi( ptr + 1 );
            if (( ptr = strchr( ptr + 1, ' ' ) ) == NULL ) continue;
            xoffset = atoi( ptr + 1 );
            if (( ptr = strchr( ptr + 1, ' ' ) ) == NULL ) continue;
            yoffset = atoi( ptr + 1 );
        }
        else if ( strncmp( line, "BITMAP", 6 ) == 0 )
        {
            /* Read bitmap data */
            if ( encoding >= 0 && encoding < 256 && height > 0 )
            {
                font->glyph[encoding].xadvance = xadvance;
                font->glyph[encoding].yadvance = yadvance;
                font->glyph[encoding].xoffset  = xoffset;
                font->glyph[encoding].yoffset  = -yoffset - height;

                if ( minyoffset > -yoffset - height ) minyoffset = -yoffset - height;

                error = 1;
                font->glyph[encoding].bitmap = bitmap_new( encoding, width, height, 1 );
                if ( font->glyph[encoding].bitmap == 0 ) break;
                bitmap_add_cpoint( font->glyph[encoding].bitmap, 0, 0 ) ;

                if ( font->maxwidth < width ) font->maxwidth = width;
                if ( font->maxheight < height ) font->maxheight = height;

                for ( y = 0 ; y < height ; y++ )
                {
                    if ( !( len = file_gets( fp, line, 2047 ) ) ) break;
                    if ( line[len-1] == '\n' ) line[len-1] = '\0';
                    ptr  = line;
                    optr = ( Uint8 * )font->glyph[encoding].bitmap->data + font->glyph[encoding].bitmap->pitch * y;

                    for ( x = 0 ; x < width ; x += 8 )
                    {
                        if ( !ptr[0] || !ptr[1] ) break;
                        *optr++ = nibbleh[ptr[0]] | nibblel[ptr[1]];
                        ptr += 2;
                    }
                }
                if ( y != height ) break;
                error = 0;
            }
        }
    }

    file_close( fp );

    if ( error )
    {
        return -1;
    }

    /* Adjust yoffsets to positive */

    for ( i = 0 ; i < 256 ; i++ ) font->glyph[i].yoffset -= minyoffset;

    if ( font->glyph[32].xadvance == 0 ) font->glyph[32].xadvance = font->glyph['j'].xadvance;

    fonts[font_count] = font ;
    return font_count++ ;
}

/* --------------------------------------------------------------------------- */
