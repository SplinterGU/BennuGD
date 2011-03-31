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

#ifndef __SCROLL_H
#define __SCROLL_H

/* --------------------------------------------------------------------------- */

#define C_SCROLL    1

/* --------------------------------------------------------------------------- */

#ifndef __GNUC__
#define __PACKED
#define inline __inline
#else
#define __PACKED __attribute__ ((packed))
#endif

#ifdef _MSC_VER
#pragma pack(push, 1)
#endif

typedef struct _scrolldata
{
    int fileid ;
    int graphid ;
    int backid ;
    REGION * region ;
    int flags ;

    int x0, y0 ;
    int posx0, posy0 ;
    int x1, y1 ;
    int z ;

    INSTANCE * camera ;

    int ratio ;
    int speed ;
    REGION * region1 ;
    REGION * region2 ;

    int active ;

    struct _scrolldata * follows ;
}
__PACKED
scrolldata ;

typedef struct _scroll_Extra_data
{
    int32_t x0 ;
    int32_t y0 ;
    int32_t x1 ;
    int32_t y1 ;
    int32_t z ;
    int32_t camera ;
    int32_t ratio ;
    int32_t speed ;
    int32_t region1 ;
    int32_t region2 ;
    int32_t flags1 ;
    int32_t flags2 ;
    int32_t follows ;
    int32_t reserved[7] ;       /* First reserved dword point to internal scrolldata struct */
}
__PACKED
SCROLL_EXTRA_DATA ;

#ifdef _MSC_VER
#pragma pack(pop)
#endif

/* --------------------------------------------------------------------------- */

extern scrolldata  scrolls[10] ;

/* --------------------------------------------------------------------------- */

extern void scroll_start( int n, int fileid, int graphid, int backid, int region, int flags ) ;
extern void scroll_stop( int n ) ;
extern void scroll_update( int n );
extern void scroll_draw( int n, REGION * clipping ) ;
extern void scroll_region( int n, REGION * r ) ;

/* --------------------------------------------------------------------------- */

#endif
