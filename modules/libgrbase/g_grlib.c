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

#include <stdlib.h>
#include <string.h>
#include "libgrbase.h"

#include <assert.h>

/* --------------------------------------------------------------------------- */

int lib_count = 0 ;
GRLIB * syslib = 0 ;

static GRLIB * libs[ MAXLIBS ] ;
static int lib_nextid = 0 ;

static GRLIB * grlib_create() ;

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : grlib_new
 *
 *  Create a new library
 *
 *  PARAMS :
 *  None
 *
 *  RETURN VALUE :
 *      ID of the new library or -1 if error
 */

int grlib_new()
{
    GRLIB * lib = grlib_create() ;
    int i ;
    if ( !lib ) return -1;

    if ( lib_nextid == MAXLIBS && lib_count == lib_nextid )
    {
        free( lib->maps );
        free( lib );
        return -1 ;
    }

    lib_count++ ;

    if ( lib_nextid < MAXLIBS && !libs[ lib_nextid ] )
    {
        libs[ lib_nextid ] = lib ;
        return lib_nextid++ ;
    }

    for ( i = lib_nextid + 1; i < MAXLIBS && libs[ i ]; i++ );

    if ( lib_nextid == MAXLIBS ) for ( i = 0; i < lib_nextid && libs[ i ]; i++ ) ;

    if ( i != MAXLIBS && !libs[ i ] )
    {
        libs[ i ] = lib ;
        lib_nextid = i + 1;

        return i ;
    }

    free( lib->maps );
    free( lib );

    return -1 ; // No memory
}

/* --------------------------------------------------------------------------- */
/* Static convenience function */

static GRLIB * grlib_create()
{
    GRLIB * lib ;

    lib = ( GRLIB * ) malloc( sizeof( GRLIB ) ) ;
    if ( !lib ) return 0 ;

    lib->maps = ( GRAPH ** ) calloc( 32, sizeof( GRAPH * ) ) ;
    if ( !lib->maps )
    {
        free( lib );
        return 0 ;
    }

    lib->name[ 0 ] = 0 ;
    lib->map_reserved = 32 ;
    return lib ;
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : grlib_get
 *
 *  Get a pointer to an library in memory given the internal ID
 *
 *  PARAMS :
 *  libid   Library code
 *
 *  RETURN VALUE :
 *      Pointer to the object required or NULL if it does not exist
 */

GRLIB * grlib_get( int libid )
{
    if ( libid < 0 || libid >= lib_nextid ) return 0 ;
    return libs[ libid ] ;
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : grlib_destroy
 *
 *  Remove a library and all its maps from memory
 *
 *  PARAMS :
 *  libid   ID of the library
 *
 *  RETURN VALUE :
 *      0 if there wasn't a map with this code, 1 otherwise
 */

void grlib_destroy( int libid )
{
    int i ;
    GRLIB * lib = grlib_get( libid ) ;
    if ( !lib ) return ;

    libs[ libid ] = 0 ;

    for ( i = 0; i < lib->map_reserved; i++ ) bitmap_destroy( lib->maps[ i ] ) ;

    free( lib->maps ) ;
    free( lib ) ;

    if ( lib_nextid == libid + 1 ) lib_nextid-- ;
    lib_count-- ;
    if ( !lib_count ) lib_nextid = 0 ;
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : grlib_unload_map
 *
 *  Remove a map from a library, if present
 *
 *  PARAMS :
 *  libid   ID of the library
 *  mapcode   ID of the map
 *
 *  RETURN VALUE :
 *      0 if there wasn't a map with this code, 1 otherwise
 */

int grlib_unload_map( int libid, int mapcode )
{
    GRLIB * lib ;

    if ( !libid || mapcode < 1 || mapcode > 999 )
        lib = syslib ;
    else
        lib = grlib_get( libid ) ;

    if ( lib == NULL ) return 0 ;

    if ( lib->map_reserved <= mapcode ) return 0 ;
    if ( !lib->maps[ mapcode ] ) return 0 ;

    bitmap_destroy( lib->maps[ mapcode ] ) ;
    lib->maps[ mapcode ] = 0 ;
    return 1 ;
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : grlib_add_map
 *
 *  Add a map to a graphic library. If any map with the same code
 *  is in the library, it will be unload first
 *
 *  PARAMS :
 *  lib    Pointer to the library
 *  map    Pointer to the graphic
 *         (fill map->code first!)
 *
 *  RETURN VALUE :
 *      -1 if error, a number >= 0 otherwise
 */

int grlib_add_map( int libid, GRAPH * map )
{
    GRLIB * lib ;

    if ( !libid || map->code < 1 || map->code > 999 )
        lib = syslib ;
    else
        lib = grlib_get( libid ) ;

    if ( !lib ) return -1 ;
    if ( map->code < 0 ) return -1 ;

    if ( map->code > 0 ) grlib_unload_map( libid, map->code ) ;

    if ( lib->map_reserved <= map->code )
    {
        GRAPH ** lmaps;
        int new_reserved = ( map->code & ~0x001F ) + 32 ;

        lmaps = ( GRAPH ** ) realloc( lib->maps, sizeof( GRAPH* ) * new_reserved ) ;
        if ( !lmaps ) return -1; // No memory
        lib->maps = lmaps;

        memset( lib->maps + lib->map_reserved, 0, ( new_reserved - lib->map_reserved ) * sizeof( GRAPH * ) ) ;
        lib->map_reserved = new_reserved ;
    }

    lib->maps[ map->code ] = map ;

    return map->code ;
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : bitmap_get
 *
 *  Get a bitmap using the DIV syntax (libcode, mapcode)
 *
 *  PARAMS :
 *  libid   Library code or 0 for system/global bitmap
 *  mapcode   Code of the bitmap
 *
 *  RETURN VALUE :
 *      Pointer to the graphic required or NULL if not found
 *
 *  (0, -1) gets the scrbitmap graphic (undocumented!)
 *  (0,  0) gets the background
 *
 */

GRAPH * bitmap_get( int libid, int mapcode )
{
    GRLIB * lib = NULL ;

    if ( !libid )
    {
        /* Using (0, 0) we can get the background map */
        if ( !mapcode ) return background ;

        /* Using (0, -1) we can get the screen bitmap (undocumented bug/feature) */
        if ( mapcode == -1 ) return scrbitmap ;
    }

    /* Get the map from the system library
     * (the only one that can have more than 1000 maps)
     */
    if ( !libid || mapcode > 999 ) lib = syslib ;

    if ( !lib ) lib = grlib_get( libid ) ;

    /* Get the map from a library */

    if ( lib && lib->map_reserved > mapcode ) return lib->maps[ mapcode ] ;

    return 0 ;
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : grlib_init
 *
 *  Create the system library. This should be called at program startup.
 *
 *  PARAMS :
 *  None
 *
 *  RETURN VALUE :
 *      None
 *
 */

void grlib_init()
{
    if ( !syslib ) syslib = grlib_create() ;
}

/* --------------------------------------------------------------------------- */
