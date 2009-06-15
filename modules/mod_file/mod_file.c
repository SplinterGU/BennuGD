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

#include "bgddl.h"

#include "arrange.h"
#include "files.h"
#include "xstrings.h"
#include "dcb.h"
#include "varspace_file.h"

/* ----------------------------------------------------------------- */

DLCONSTANT  __bgdexport( mod_file, constants_def)[] =
{
    { "O_READ"      , TYPE_INT, 0  },
    { "O_READWRITE" , TYPE_INT, 1  },
    { "O_RDWR"      , TYPE_INT, 1  },
    { "O_WRITE"     , TYPE_INT, 2  },
    { "O_ZREAD"     , TYPE_INT, 3  },
    { "O_ZWRITE"    , TYPE_INT, 4  },

    { "SEEK_SET"    , TYPE_INT, 0  },
    { "SEEK_CUR"    , TYPE_INT, 1  },
    { "SEEK_END"    , TYPE_INT, 2  },

    { NULL          , 0       , 0  }
} ;

/* ----------------------------------------------------------------- */

static int modfile_save( INSTANCE * my, int * params )
{
    file * fp ;
    const char * filename ;
    int result = 0 ;

    filename = string_get( params[0] ) ;
    if ( !filename ) return 0 ;

    fp = file_open( filename, "wb0" ) ;
    if ( fp )
    {
        result = savetypes( fp, ( void * )params[1], ( void * )params[2], params[3], 0 );
        file_close( fp ) ;
    }
    string_discard( params[0] ) ;
    return result ;
}

static int modfile_load( INSTANCE * my, int * params )
{
    file * fp ;
    const char * filename ;
    int result = 0 ;

    filename = string_get( params[0] ) ;
    if ( !filename ) return 0 ;

    fp = file_open( filename, "rb0" ) ;
    if ( fp )
    {
        result = loadtypes( fp, ( void * )params[1], ( void * )params[2], params[3], 0 );
        file_close( fp ) ;
    }
    string_discard( params[0] ) ;
    return result ;
}

static int modfile_fopen( INSTANCE * my, int * params )
{
    static char * ops[] = { "rb0", "r+b0", "wb0", "rb", "wb6" } ;
    int r ;

    if ( params[1] < 0 || params[1] > 4 )
        params[0] = 0 ;

    r = ( int ) file_open( string_get( params[0] ), ops[params[1]] ) ;
    string_discard( params[0] ) ;
    return r ;
}

static int modfile_fclose( INSTANCE * my, int * params )
{
    file_close(( file * )params[0] ) ;
    return 1 ;
}

static int modfile_fread( INSTANCE * my, int * params )
{
    return loadtypes(( file * )params[0], ( void * )params[1], ( void * )params[2], params[3], 0 );
}

static int modfile_fwrite( INSTANCE * my, int * params )
{
    return savetypes(( file * )params[0], ( void * )params[1], ( void * )params[2], params[3], 0 );
}

static int modfile_freadC( INSTANCE * my, int * params )
{
    return file_read(( file * )params[2], ( void * )params[0], params[1] );
}

static int modfile_fwriteC( INSTANCE * my, int * params )
{
    return file_write(( file * )params[2], ( void * )params[0], params[1] );
}

static int modfile_fseek( INSTANCE * my, int * params )
{
    return file_seek(( file * )params[0], params[1], params[2] ) ;
}

static int modfile_frewind( INSTANCE * my, int * params )
{
    file_rewind(( file * )params[0] ) ;
    return 1;
}

static int modfile_ftell( INSTANCE * my, int * params )
{
    return file_pos(( file * )params[0] ) ;
}

static int modfile_filelength( INSTANCE * my, int * params )
{
    return file_size(( file * )params[0] ) ;
}

static int modfile_fputs( INSTANCE * my, int * params )
{
    char * str = ( char * ) string_get( params[1] );
    int r = file_puts(( file * )params[0], str ) ;
    if ( str[strlen( str )-1] != '\n' ) file_puts(( file * )params[0], "\r\n" ) ;
    /*    int r = file_puts ((file *)params[0], string_get(params[1])) ; */
    string_discard( params[1] ) ;
    return r ;
}

static int modfile_fgets( INSTANCE * my, int * params )
{
    char buffer[1025] ;
    int len, done = 0 ;
    int str = string_new( "" );

    while ( !done )
    {
        len = file_gets(( file * )params[0], buffer, sizeof( buffer ) - 1) ;
        if ( len < 1 ) break;

        if ( buffer[len-1] == '\r' || buffer[len-1] == '\n' )
        {
            len--;
            if ( len && ( buffer[len-1] == '\r' || buffer[len-1] == '\n' )) len--;
            buffer[len] = '\0' ;
            done = 1;
        }
        string_concat( str, buffer );
    }
    string_use( str ) ;
    return str ;
}

static int modfile_file( INSTANCE * my, int * params )
{
    char buffer[1025] ;
    int str = string_new( "" ) ;
    file * f ;
    int l;

    f = file_open( string_get( params[0] ), "rb" ) ;
    string_discard( params[0] ) ;

    if ( f )
    {
        while ( !file_eof( f ) )
        {
            l = file_read( f, buffer, sizeof( buffer ) - 1 ) ;
            buffer[l] = '\0' ;
            if ( l )
            {
                string_concat( str, buffer ) ;
                buffer[0] = '\0' ;
            }
            else
                break;
        }
        file_close( f ) ;
    }

    string_use( str ) ;

    return str ;
}

static int modfile_feof( INSTANCE * my, int * params )
{
    return file_eof(( file * )params[0] ) ;
}

static int modfile_exists( INSTANCE * my, int * params )
{
    int r = file_exists( string_get( params[0] ) ) ;
    string_discard( params[0] ) ;
    return r ;
}

static int modfile_remove( INSTANCE * my, int * params )
{
    int r = file_remove( string_get( params[0] ) ) ;
    string_discard( params[0] ) ;
    return r ;
}

static int modfile_move( INSTANCE * my, int * params )
{
    int r = file_move( string_get( params[0] ), string_get( params[1] ) ) ;
    string_discard( params[1] ) ;
    string_discard( params[0] ) ;
    return r ;
}

/* ----------------------------------------------------------------- */
/* Declaracion de funciones                                          */

DLSYSFUNCS  __bgdexport( mod_file, functions_exports)[] =
{
    /* Ficheros */
    { "SAVE"        , "SV++" , TYPE_INT         , modfile_save        },
    { "LOAD"        , "SV++" , TYPE_INT         , modfile_load        },
    { "FOPEN"       , "SI"   , TYPE_INT         , modfile_fopen       },
    { "FCLOSE"      , "I"    , TYPE_INT         , modfile_fclose      },
    { "FREAD"       , "IV++" , TYPE_INT         , modfile_fread       },
    { "FREAD"       , "PII"  , TYPE_INT         , modfile_freadC      },
    { "FWRITE"      , "IV++" , TYPE_INT         , modfile_fwrite      },
    { "FWRITE"      , "PII"  , TYPE_INT         , modfile_fwriteC     },
    { "FSEEK"       , "III"  , TYPE_INT         , modfile_fseek       },
    { "FREWIND"     , "I"    , TYPE_UNDEFINED   , modfile_frewind     },
    { "FTELL"       , "I"    , TYPE_INT         , modfile_ftell       },
    { "FLENGTH"     , "I"    , TYPE_INT         , modfile_filelength  },
    { "FPUTS"       , "IS"   , TYPE_INT         , modfile_fputs       },
    { "FGETS"       , "I"    , TYPE_STRING      , modfile_fgets       },
    { "FEOF"        , "I"    , TYPE_INT         , modfile_feof        },
    { "FILE"        , "S"    , TYPE_STRING      , modfile_file        },
    { "FEXISTS"     , "S"    , TYPE_INT         , modfile_exists      } ,
    { "FILE_EXISTS" , "S"    , TYPE_INT         , modfile_exists      } ,
    { "FREMOVE"     , "S"    , TYPE_INT         , modfile_remove      } ,
    { "FMOVE"       , "SS"   , TYPE_INT         , modfile_move        } ,
    { 0             , 0      , 0                , 0                   }
};
