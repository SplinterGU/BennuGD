/*
 *  Copyright © 2006-2009 SplinterGU (Fenix/Bennugd)
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
/*
 * INCLUDES
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#include "bgdrtm.h"
#include "offsets.h"
#include "dcb.h"
#include "sysprocs_p.h"
#include "xstrings.h"

/* --------------------------------------------------------------------------- */

char * appname = NULL;

/* --------------------------------------------------------------------------- */

int debug     = 0;  /* 1 if running in debug mode      */

/* --------------------------------------------------------------------------- */

/* os versions */
#ifdef _WIN32
#define _OS_ID          OS_WIN32
#endif
#ifdef TARGET_LINUX
#define _OS_ID          OS_LINUX
#endif
#ifdef TARGET_BEOS
#define _OS_ID          OS_BEOS
#endif
#ifdef TARGET_MAC
#define _OS_ID          OS_MACOS
#endif
#ifdef TARGET_GP32
#define _OS_ID          OS_GP32
#endif
#ifdef TARGET_DC
#define _OS_ID          OS_DC
#endif
#ifdef TARGET_BSD
#define _OS_ID          OS_BSD
#endif
#ifdef TARGET_GP2X
#ifdef _OS_ID
#undef _OS_ID
#endif
#define _OS_ID          OS_GP2X
#endif
#ifdef TARGET_GP2X_WIZ
#ifdef _OS_ID
#undef _OS_ID
#endif
#define _OS_ID          OS_GP2X_WIZ
#endif

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : strncmpi
 *
 *  Compare two strings (case-insensitive version)
 *
 *  PARAMS:
 *      str1    first string
 *      str2    second string
 *
 *  RETURN VALUE:
 *      Result of the comparison
 */

int strncmpi( char * str1, char * str2, int sz )
{
    while (( *str1 || *str2 ) && sz )
    {
        if ( toupper( *str1 ) != toupper( *str2 ) ) return toupper( *str1 ) - toupper( *str2 );
        str1++; str2++; sz--;
    }

    return 0 ;
}

/* --------------------------------------------------------------------------- */

void bgdrtm_entry( int argc, char * argv[] )
{
    int i;
    int * args = (int *)&GLODWORD( ARGV_TABLE );

    GLODWORD( ARGC ) = argc ;

    for ( i = 0 ; i < argc && i < 32; i++ )
    {
        args[i] = string_new( argv[i] ) ;
        string_use( args[i] ) ;
    }

    GLODWORD( OS_ID ) = _OS_ID ;
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : bgdrtm_exit
 *
 *  Exits from the program cleanly ending operations
 *
 *  PARAMS:
 *      INT n: ERROR LEVEL to return to OS
 *
 *  RETURN VALUE:
 *      No value
 *
 */

void bgdrtm_exit( int exit_value )
{
    int n;
    /* Finalize all modules */
    if ( module_finalize_count )
        for ( n = 0; n < module_finalize_count; n++ )
            module_finalize_list[n]();
    exit( exit_value ) ;
}

/* --------------------------------------------------------------------------- */
