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
#include <string.h>
#ifdef TARGET_BEOS
#include <posix/assert.h>
#else
#include <assert.h>
#endif

#include "bgdc.h"

/* ---------------------------------------------------------------------- */
/* Gestor de cadenas                                                      */
/* ---------------------------------------------------------------------- */

char * string_mem = 0 ;
int string_allocated = 0 ;
int string_used = 0 ;

int * string_offset = 0 ;
int string_max = 0 ;
int string_count = 0 ;

int autoinclude = 0 ;

void string_init()
{
    string_mem = ( char * ) calloc( 4096, sizeof( char ) ) ;
    string_allocated = 4096 ;
    string_used = 1 ;
    string_count = 1 ;
    string_offset = ( int * ) calloc( 1024, sizeof( int ) ) ;
    string_max = 1024 ;
    string_mem[ 0 ] = 0 ;
    string_offset[ 0 ] = 0 ;
}

void string_alloc( int bytes )
{
    string_mem = ( char * ) realloc( string_mem, string_allocated += bytes ) ;
    if ( !string_mem )
    {
        fprintf( stdout, "string_alloc: out of memory\n" ) ;
        exit( 1 ) ;
    }
}

void string_dump( void ( *wlog )( const char *fmt, ... ) )
{
    int i ;
    printf( "\n---- %d strings ----\n\n", string_count ) ;
    for ( i = 0 ; i < string_count ; i++ )
        printf( "%4d: %s\n", i, string_mem + string_offset[ i ] ) ;
}

int string_new( const char * text )
{
    int len = strlen( text ) + 1 ;

    if ( string_count == string_max )
    {
        string_max += 1024 ;
        string_offset = ( int * ) realloc( string_offset, string_max * sizeof( int ) ) ;
        if ( string_offset == 0 )
        {
            fprintf( stdout, "Too many strings\n" ) ;
            exit( 1 ) ;
        }
    }

    while ( string_used + len >= string_allocated ) string_alloc( 1024 ) ;

    string_offset[ string_count ] = string_used ;
    strcpy( string_mem + string_used, text ) ;
    string_used += len ;
    return string_count++ ;
}


char * validchars = "\\/.";
char * invalidchars = \
        "\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F" \
        "\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1A\x1B\x1C\x1D\x1E\x1F" \
        "*?\"<>|";

int check_for_valid_pathname( char * pathname )
{
    int n, l;

    if ( !pathname || ( l = strlen( pathname ) ) > __MAX_PATH ) return 0;

#if WIN32
    /* Only ':' with this sintax: "L:..." */
    if ( pathname[ 0 ] == ':' || ( l > 2 && strchr( pathname + 2, ':' ) ) ) return 0;
#endif

    /* Some invalid character? */
    for ( n = 0; n < strlen( invalidchars ); n++ )
        if ( strchr( pathname, invalidchars[ n ] ) ) return 0;

    return 1;
}

int string_compile( const char ** source )
{
    char c = *( *source ) ++, conv ;
    const char * ptr ;

    if ( string_count == string_max )
    {
        string_max += 1024 ;
        string_offset = ( int * ) realloc( string_offset, string_max * sizeof( int ) ) ;
        if ( string_offset == 0 )
        {
            fprintf( stdout, "Too many strings\n" ) ;
            exit( 1 ) ;
        }
    }

    string_offset[ string_count ] = string_used ;

    while ( *( *source ) )
    {
        if ( *( *source ) == c )   /* Termina la string? */
        {
            ( *source ) ++ ;
            if ( *( *source ) == c )  /* Comienza una nueva? (esto es para strings divididas) */
            {
                ( *source ) ++ ;
            }
            else
            {
                /* Elimino todos los espacios para buscar si hay otra string, esto es para strings divididas */
                ptr = ( *source ) ;
                while ( ISSPACE( *ptr ) )
                {
                    if ( *ptr == '\n' ) line_count++ ;
                    ptr++ ;
                }
                /* Si despues de saltar todos los espacios, no tengo un delimitador de string, salgo */
                if ( *ptr != c )
                {
                    ( *source ) = ptr; /* Fix: Splinter, por problema con numeracion de lineas */
                    break ;
                }

                /* Obtengo delimitador de string, me posiciono en el caracter siguiente, dentro de la string */
                ( *source ) = ptr + 1 ;
                continue ;
            }
        }
        else if ( *( *source ) == '\n' )
        {
            line_count++ ;
            string_mem[ string_used++ ] = '\n' ;

            ( *source ) ++ ;
        }
        else
        {
            conv = convert( *( *source ) ) ;
            string_mem[ string_used++ ] = conv ;

            ( *source ) ++ ;
        }

        if ( string_used >= string_allocated )
            string_alloc( 1024 ) ;
    }

    string_mem[ string_used++ ] = 0 ;
    if ( string_used >= string_allocated )
        string_alloc( 1024 ) ;

    /* Hack: añade el posible fichero al DCB */

    if ( autoinclude && check_for_valid_pathname( string_mem + string_offset[ string_count ] ) )
        dcb_add_file( string_mem + string_offset[ string_count ] ) ;

    return string_count++ ;
}

const char * string_get( int code )
{
    assert( code < string_count && code >= 0 ) ;
    return string_mem + string_offset[ code ] ;
}

