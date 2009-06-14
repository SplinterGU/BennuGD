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

#include <stdio.h>
#include <stdlib.h>

#include "bgdc.h"

#include "varspace.h"

/* ---------------------------------------------------------------------- */
/* Gestor de constantes                                                   */
/* ---------------------------------------------------------------------- */

static CONSTANT * constants ;
static int constants_used ;
static int constants_reserved ;

void constants_init()
{
    constants = ( CONSTANT * ) calloc( 16, sizeof( CONSTANT ) );
    constants_reserved = 16 ;
    constants_used = 0 ;
}

void constants_alloc( int count )
{
    constants = ( CONSTANT * ) realloc( constants, ( constants_reserved += count ) * sizeof( CONSTANT ) ) ;
    if ( !constants )
    {
        fprintf( stdout, "constants_alloc: out of memory\n" ) ;
        exit( 1 ) ;
    }
}

CONSTANT * constants_search( int code )
{
    int i ;

    for ( i = 0 ; i < constants_used ; i++ ) if ( constants[i].code == code ) return &constants[i] ;
    return 0 ;
}

void constants_add( int code, TYPEDEF type, int value )
{
    CONSTANT * c;

    if ( constants_used == constants_reserved ) constants_alloc( 16 ) ;

    if ( varspace_search( &global, code ) )
    {
        token.code = code;
        token.type = IDENTIFIER;
        compile_error( MSG_VARIABLE_REDECLARED_AS_CONSTANT ) ;
    }

    if ( varspace_search( &local, code ) )
    {
        token.code = code;
        token.type = IDENTIFIER;
        compile_error( MSG_VARIABLE_REDECLARED_AS_CONSTANT ) ;
    }

    if (( c = constants_search( code ) ) && ( !typedef_is_equal( c->type, type ) || c->value != value ) )
    {
        token.code = string_new( identifier_name( code ) );
        token.type = STRING;
        compile_error( "Constant redefined" );
    }

    constants[constants_used].code = code ;
    constants[constants_used].type = type ;
    constants[constants_used].value = value ;
    constants_used++ ;
}

void constants_dump()
{
    int i ;

    printf( "---- %d constants of %d ----\n", constants_used, constants_reserved ) ;
    for ( i = 0 ; i < constants_used ; i++ )
        printf( "%4d: %-16s= %d\n", i,
                identifier_name( constants[i].code ), constants[i].value ) ;
}
