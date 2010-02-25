/*
 *  Copyright © 2006-2010 SplinterGU (Fenix/Bennugd)
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "bgddl.h"
#include "files.h"
#include "xstrings.h"

#ifdef TARGET_MAC
#include <SDL/SDL.h>
#else
#include <SDL.h>
#endif

#include <time.h>

/* --------------------------------------------------------------------------- */
/* Timer                                                                       */

static int modtime_get_timer( INSTANCE * my, int * params )
{
    return SDL_GetTicks() ;
}

/* --------------------------------------------------------------------------- */
/* Hora del día                                                                */

static int modtime_time( INSTANCE * my, int * params )
{
    return time( 0 ) ;
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : modtime_ftime
 *
 *  Returns parts of the date
 *
 *  PARAMS:
 *      no params
 *
 *  RETURN VALUE:
 *      pointer to a string value...
 *
 */

static int modtime_ftime( INSTANCE * my, int * params )
{
    char buffer[128] ;
    char * format ;
    struct tm * t ;
    int ret ;
    time_t tim ;
    char * base ;

#ifdef _WIN32
    /* aux buffer to make all changes... */
    char aux[128] ;
    unsigned char pos ;
#endif

    format = base = strdup( string_get( params[0] ) ) ;
    string_discard( params[0] ) ;

#ifdef _WIN32
    /* Addapting win32 strftime formats to linux formats */
    /* HEAVY PATCH... :( */
    pos = 0 ;
    while ( *format && pos < 127 )
    {
        switch ( *format )
        {
            case '%': /* MIGHT NEED CONVERSION... */
                aux[pos] = *format ;
                pos++ ;
                format++ ;
                switch ( *format )
                {
                    case 'e':
                        aux[pos++] = '#' ;
                        aux[pos] = 'd' ;
                        break ;
                    case 'l':
                        aux[pos++] = '#' ;
                        aux[pos] = 'I' ;
                        break ;
                    case 'k':
                        aux[pos++] = '#' ;
                        aux[pos] = 'H' ;
                        break ;
                    case 'P':
                        aux[pos] = 'p' ;
                        break ;

                    case 'C':
                        aux[pos++] = '%' ;
                        aux[pos++] = *format ;
                        aux[pos++] = '%' ;
                        aux[pos] = 'Y' ;
                        break ;

                    case 'u':
                        aux[pos++] = '%' ;
                        aux[pos++] = *format ;
                        aux[pos++] = '%' ;
                        aux[pos] = 'w' ;
                        break ;

                    case '%':   //MUST BE %%%% TO KEEP 2 IN POSTPROCESS
                        aux[pos++] = '%' ;
                        aux[pos++] = '%' ;
                        aux[pos] = '%' ;
                        break ;

                    default:
                        aux[pos] = *format ;
                        break ;
                }
                break ;

            default: aux[pos] = *format ;
                break ;
        }
        format++ ;
        pos++ ;
    }
    aux[pos] = 0 ;
    format = aux ;
#endif

    tim = ( time_t ) params[1] ;
    t = localtime( &tim ) ;
    strftime( buffer, sizeof( buffer ), format, t ) ;

#ifdef _WIN32
    /* win32 postprocess */
    aux[0] = '\0' ;
    format = buffer ;
    pos = 0 ;
    while ( *format )
    {
        switch ( *format )
        {
            case '%':
                format++ ;
                switch ( *format )
                {
                    case 'u':
                        format++ ;
                        if ( *format == '0' ) *format = '7' ;
                        aux[pos] = *format ;
                        break ;

                    case 'C':
                        format++ ;
                        aux[pos++] = *format ;
                        format++ ;
                        aux[pos] = *format ;
                        format++ ;
                        format++ ;
                        break ;

                    default:
                        aux[pos] = *format ;
                        break ;
                }
                break ;

            default:
                aux[pos] = *format ;
                break ;
        }
        format++ ;
        pos++;
    }
    aux[pos] = '\0' ;
    strcpy( buffer, aux ) ;
#endif

    ret = string_new( buffer ) ;
    string_use( ret ) ;

    free( base ) ;

    return ret ;
}

/* --------------------------------------------------------------------------- */
/* Declaracion de funciones                                                    */

DLSYSFUNCS __bgdexport( mod_time, functions_exports )[] =
{
    /* Fecha/Hora */
    { "GET_TIMER"   , ""    , TYPE_INT      , modtime_get_timer     },
    { "TIME"        , ""    , TYPE_INT      , modtime_time          },
    { "FTIME"       , "SI"  , TYPE_STRING   , modtime_ftime         },
    { 0             , 0     , 0             , 0                     }
};

/* --------------------------------------------------------------------------- */

void __bgdexport( mod_time, module_initialize )()
{
    if ( !SDL_WasInit( SDL_INIT_TIMER ) ) SDL_InitSubSystem( SDL_INIT_TIMER );
}

/* --------------------------------------------------------------------------- */

void __bgdexport( mod_time, module_finalize )()
{
    if ( SDL_WasInit( SDL_INIT_TIMER ) ) SDL_QuitSubSystem( SDL_INIT_TIMER );
}

/* --------------------------------------------------------------------------- */
