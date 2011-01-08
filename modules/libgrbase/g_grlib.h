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

#ifndef __GRLIB_H
#define __GRLIB_H

#define MAXLIBS 1024

typedef struct _grlib
{
    GRAPH ** maps ;
    int map_reserved ;
    char name[ 64 ];
}
GRLIB ;

extern GRLIB * syslib ;

extern GRAPH * bitmap_get( int libid, int mapcode ) ;
extern GRLIB * grlib_get( int libid ) ;
extern void grlib_init() ;
extern int grlib_new() ;
extern void grlib_destroy( int libid ) ;
extern int grlib_add_map( int libid, GRAPH * map ) ;
extern int grlib_unload_map( int libid, int mapcode ) ;

#endif
