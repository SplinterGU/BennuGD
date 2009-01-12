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

/*
 * FILE        : dirs.c
 * DESCRIPTION : directory functions
 *
 * HISTORY:
 *
 */

#include "bgdrtm.h"
#include "dirs.h"
#include "xstrings.h"
#include <string.h>
#include <stdlib.h>

/*
 *  FUNCTION : dir_path_convert
 *
 *  Convert a path to the valid OS format
 *
 *  PARAMS :
 *      char * path:    path to convert
 *
 *  RETURN VALUE :
 *      char *:         converted path
 *
 */

char * dir_path_convert( const char * dir )
{

    char *c, *p ;

    p = strdup( dir ) ;
    c = p ;
    // Convert characters
    while ( *p )
    {
        if ( *p == PATH_CHAR_ISEP ) *p = PATH_CHAR_SEP ;
        p++ ;
    }
    return c;
}


/*
 *  FUNCTION : dir_current
 *
 *  Retrieve current directory
 *
 *  PARAMS :
 *
 *  RETURN VALUE :
 *      STRING ID pointing to a system string with the current dir
 *
 */

char * dir_current( void )
{

    char dir[1024] ; /* buffer to the directory */
    char * c = getcwd( dir, sizeof( dir ) ) ;
    return c ? strdup( c ) : NULL ;
}

/*
 *  FUNCTION : dir_change
 *
 *  Retrieve current directory
 *
 *  PARAMS :
 *      char * dir:     the new current directory
 *
 *  RETURN VALUE :
 *      0           - FAILURE
 *      NON_ZERO    - SUCCESS
 *
 */

int dir_change( const char * dir )
{

    char *c = dir_path_convert( dir ) ;
    int r = chdir( c ) ;
    free( c ) ;
    return r ;
}


/*
 *  FUNCTION : dir_create
 *
 *  Retrieve current directory
 *
 *  PARAMS :
 *      char * dir:     the directory to create
 *
 *  RETURN VALUE :
 *      0           - FAILURE
 *      NON_ZERO    - SUCCESS
 *
 */

int dir_create( const char * dir )
{

    char *c = dir_path_convert( dir ) ;
    int r ;

#ifdef WIN32
    r = mkdir( c ) ;
#else
    r = mkdir( c, 0777 ) ;
#endif
    free( c ) ;
    return r ;
}

/*
 *  FUNCTION : dir_delete
 *
 *  Retrieve current directory
 *
 *  PARAMS :
 *      char * dir:     the directory to delete
 *
 *  RETURN VALUE :
 *      0           - FAILURE
 *      NON_ZERO    - SUCCESS
 *
 */

int dir_delete( const char * dir )
{

    char *c = dir_path_convert( dir ) ;
    int r = rmdir( c ) ;
    free( c ) ;
    return r ;
}

/*
 *  FUNCTION : dir_deletefile
 *
 *  Remove a given file
 *
 *  PARAMS :
 *      char * filename: the file to delete
 *
 *  RETURN VALUE :
 *      0           - FAILURE
 *      NON_ZERO    - SUCCESS
 *
 */

int dir_deletefile( const char * filename )
{

    char *c = dir_path_convert( filename ) ;
    int r = unlink( c ) ;
    free( c ) ;
    return ( r == -1 ) ? 0 : 1 ;
}
