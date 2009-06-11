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
 * FILE        : i_copy.c
 * DESCRIPTION : Struct copy functions based on varspace type info
 *
 * HISTORY:      0.85 - First version
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <math.h>
#include <time.h>

#include "xstrings.h"
#include "bgdrtm.h"

static int copytype( void * dst, void * src, DCB_TYPEDEF * var );

/*
 *  FUNCTION : copyvars
 *
 *  Copy data using a varspace's type information. It updates the
 *  destination pointer while walking the variables.
 *
 *  PARAMS :
 *  dst    Pointer to the destination memory
 *  src    Pointer to the data
 *  var    Pointer to the type array
 *  nvars   Number of variables (length of var array)
 *
 *  RETURN VALUE :
 *      Number of bytes actually copied
 *
 */

int copyvars( void * dst, void * src, DCB_VAR * var, int nvars )
{
    int result = 0;
    int partial;

    for ( ; nvars > 0; nvars--, var++ )
    {
        partial = copytype( dst, src, &var->Type );
        src = (( uint8_t* )src ) + partial;
        dst = (( uint8_t* )dst ) + partial;
        result += partial;
    }
    return result;
}


/*
 *  FUNCTION : copytypes
 *
 *  Copy data from memory using type information stored in memory
 *
 *  PARAMS :
 *  dst    Pointer to the destination memory
 *  src    Pointer to the source
 *  var    Pointer to the type array
 *  nvars   Number of variables (length of var array)
 *
 *  RETURN VALUE :
 *      Number of bytes actually coppied
 *
 */

int copytypes( void * dst, void * src, DCB_TYPEDEF * var, int nvars, int reps )
{
    int result = 0;
    int partial;
    DCB_TYPEDEF * _var = var;
    int _nvars = nvars ;

    for ( ; reps > 0; reps-- )
    {
        var = _var;
        nvars = _nvars;
        for ( ; nvars > 0; nvars--, var++ )
        {
            partial = copytype( dst, src, var );
            result += partial;
            src = (( uint8_t* )src ) + partial;
            dst = (( uint8_t* )dst ) + partial;
        }
    }
    return result;
}

/*
 *  FUNCTION : copytype
 *
 *  Copy one variable using the given type information.
 *
 *  PARAMS :
 *  dst    Pointer to the file object
 *  src    Pointer to the data
 *  var    Pointer to the variable type
 *
 *  RETURN VALUE :
 *      Number of bytes actually written
 *
 */

static int copytype( void * dst, void * src, DCB_TYPEDEF * var )
{
    int count  = 1;
    int result = 0;
    int n      = 0;

    for ( ;; )
    {
        switch ( var->BaseType[n] )
        {
            case TYPE_FLOAT:
            case TYPE_INT:
            case TYPE_DWORD:
            case TYPE_POINTER:
                memcpy( dst, src, 4 * count );
                return 4 * count;

            case TYPE_WORD:
            case TYPE_SHORT:
                memcpy( dst, src, 2 * count );
                return 2 * count;

            case TYPE_BYTE:
            case TYPE_SBYTE:
            case TYPE_CHAR:
                memcpy( dst, src, count );
                return count;

            case TYPE_STRING:
                while ( count-- )
                {
                    string_discard( *( int * )dst );
                    string_use( *( int * )src );
                    *(( int * )dst ) = *(( int * )src );
                    dst = (( int * )dst ) + 1;
                    src = (( int * )src ) + 1;
                    result += 4;
                }
                return result;

            case TYPE_ARRAY:
                count *= var->Count[n];
                n++;
                continue;

            case TYPE_STRUCT:
                for ( ; count ; count-- )
                {
                    int partial = copyvars( dst, src, dcb.varspace_vars[var->Members], dcb.varspace[var->Members].NVars );
                    src = (( uint8_t* )src ) + partial;
                    dst = (( uint8_t* )dst ) + partial;
                    result += partial;
                }
                break;

            default:
                fprintf( stderr, "ERROR: Runtime error - Could not copy datatype\n" ) ;
                exit( 1 );
                break;
        }
        break;
    }
    return result;
}

/**
 *  POINTER #COPY# (POINTER DEST, POINTER SRC, POINTER VARSPACE, INT VARS)
 *
 *  Copy struct data from src to dst, using the information varspace given
 **/

int bgd_copy_struct( INSTANCE * my, int * params )
{
    return ( int ) copytypes(( void * )params[0], ( void * )params[1], ( DCB_TYPEDEF * )params[2], params[3], params[4] );
}

int bgd_internal_memcopy( INSTANCE * my, int * params )
{
    memmove(( void * )params[0], ( void * )params[1], params[2] ) ;
    return 1 ;
}
