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

#ifndef __FONT_H
#define __FONT_H

#include "libgrbase.h"

/* -------------------------------------------------------------------------- */

#define MAX_FONTS       256

#define NFB_FIXEDWIDTH 1

/* -------------------------------------------------------------------------- */

enum
{
    CHARSET_ISO8859 = 0,
    CHARSET_CP850 = 1
};

/* -------------------------------------------------------------------------- */

typedef struct _font
{
    int    charset;
    uint32_t bpp;

    struct _glyph
    {
        GRAPH * bitmap;
        int     xoffset;
        int     yoffset;
        int     xadvance;
        int     yadvance;
    }
    glyph[256] ;

    int maxheight;
    int maxwidth;
}
FONT ;

/* -------------------------------------------------------------------------- */

extern unsigned char default_font[256*8];

/* -------------------------------------------------------------------------- */

extern FONT * fonts[MAX_FONTS] ;
extern int font_count ;

/* -------------------------------------------------------------------------- */

extern void gr_font_destroy( int fontid ) ;
extern FONT * gr_font_get( int id );
extern int gr_font_new() ;
extern int gr_font_newfrombitmap( char * chardata, int width, int height, int options ) ;
extern int gr_font_systemfont( char * chardata ) ;

/* -------------------------------------------------------------------------- */

#endif
