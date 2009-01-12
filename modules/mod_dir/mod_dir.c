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
#include <time.h>
#include <ctype.h>

#include "dirs.h"

/* WIN32 INCLUDES */
#ifdef WIN32
#include <windows.h>
#include <winbase.h>
#include <windef.h>
#else
#include <unistd.h>
#include <sys/utsname.h>
/* BeOS INCLUDES */
#ifdef TARGET_BEOS
#include <sys/types.h>
#endif
#include <sys/stat.h>
#include <glob.h>
#endif

#include "bgddl.h"
#include "dlvaracc.h"
#include "files.h"
#include "xstrings.h"

/* ----------------------------------------------------------------- */

enum
{
    FILE_PATH = 0,
    FILE_NAME,
    FILE_DIRECTORY,
    FILE_HIDDEN,
    FILE_READONLY,
    FILE_SIZE,
    FILE_CREATED,
    FILE_MODIFIED
} ;

/* ----------------------------------------------------------------- */
/* Definicion de variables globales (usada en tiempo de compilacion) */

char * __bgdexport( mod_dir, globals_def )=
    "STRUCT fileinfo\n"
    "    STRING path;\n"
    "    STRING name;\n"
    "    directory;\n"
    "    hidden;\n"
    "    readonly;\n"
    "    size;\n"
    "    STRING created;\n"
    "    STRING modified;\n"
    "END\n";

/* ----------------------------------------------------------------- */
/* Son las variables que se desea acceder.                           */
/* El interprete completa esta estructura, si la variable existe.    */
/* (usada en tiempo de ejecucion)                                    */

DLVARFIXUP __bgdexport( mod_dir, globals_fixup)[] =
{
    /* Nombre de variable global, puntero al dato, tamaño del elemento, cantidad de elementos */
    { "fileinfo.path" , NULL, -1, -1 },
    { "fileinfo.name" , NULL, -1, -1 },
    { "fileinfo.directory" , NULL, -1, -1 },
    { "fileinfo.hidden" , NULL, -1, -1 },
    { "fileinfo.readonly" , NULL, -1, -1 },
    { "fileinfo.size" , NULL, -1, -1 },
    { "fileinfo.created" , NULL, -1, -1 },
    { "fileinfo.modified" , NULL, -1, -1 },
    { NULL, NULL, -1, -1 }
};

/* ----------------------------------------------------------------- */
/* DIRECTORY FUNCTIONS */

static int moddir_cd( INSTANCE * my, int * params )
{
    char * d = dir_current() ;
    int r = string_new( d ) ;
    string_use( r ) ;
    if ( d ) free( d ) ;
    return r ;
}

static int moddir_chdir( INSTANCE * my, int * params )
{
    const char * d = string_get( params[ 0 ] ) ;
    return ( dir_change( d ) ) ;
}

static int moddir_mkdir( INSTANCE * my, int * params )
{
    const char * d = string_get( params[ 0 ] ) ;
    return ( dir_create( d ) ) ;
}

static int moddir_rmdir( INSTANCE * my, int * params )
{
    const char * d = string_get( params[ 0 ] ) ;
    return ( dir_delete( d ) ) ;
}

static int moddir_rm( INSTANCE * my, int * params )
{
    const char * d = string_get( params[ 0 ] ) ;
    return ( dir_deletefile( d ) ) ;
}


/*  string GLOB (STRING path)
 *
 *  Given a path with wildcards ('*' or '?' characters), returns the first
 *  file that matches and, in every next call, all matching files found
 *  until no more files exists. It then returns NIL.
 */

static int moddir_glob( INSTANCE * my, int * params )
{
#ifdef _WIN32
    static char * last_path = NULL;
    static HANDLE handle;
    const char * path = string_get( params[ 0 ] );
    WIN32_FIND_DATA data;
    SYSTEMTIME time;
    int result;
    char realpath[ __MAX_PATH ];
    char fullname[ __MAX_PATH ];
    char buffer[ 128 ];
    char * ptr;

    if ( !path )
    {
        if ( handle ) FindClose( handle );
        string_discard( params[ 0 ] );
        if ( last_path ) free( last_path );
        last_path = NULL;
        handle = NULL;
        result = string_new( "" );
        string_use( result );
        return ( result );
    }

    if ( handle && ( last_path && strcmp( last_path, path ) == 0 ) )
    {
        /* Continue last search */
        if ( !FindNextFile( handle, &data ) )
        {
            FindClose( handle );
            /* No more matches found */
            string_discard( params[ 0 ] );
            if ( last_path ) free( last_path );
            last_path = NULL;
            handle = NULL;
            result = string_new( "" );
            string_use( result );
            return ( result );
        }
    }
    else
    {
        if ( handle ) FindClose( handle );

        /* New search */
        handle = FindFirstFile( path, &data );
        if ( handle == INVALID_HANDLE_VALUE )
        {
            /* No matches found */
            string_discard( params[ 0 ] );
            if ( last_path ) free( last_path );
            last_path = NULL;
            result = string_new( "" );
            string_use( result );
            return ( result );
        }
        last_path = strdup( path );
    }

    /* Fill the FILEINFO struct */
    strcpy( fullname, path );
    ptr = fullname + strlen( fullname );
    while ( ptr >= fullname )
    {
        if ( *ptr == '\\' || *ptr == '/' )
        {
            ptr[ 1 ] = 0;
            break;
        }
        ptr--;
    }

    strcat( fullname, data.cFileName );
    GetFullPathName( fullname, __MAX_PATH, realpath, &ptr );
    if ( ptr ) * ptr = '\0';

    /* Store the file path */
    if ( GLODWORD( mod_dir, FILE_PATH ) ) string_discard( GLODWORD( mod_dir, FILE_PATH ) );
    GLODWORD( mod_dir, FILE_PATH ) = string_new( realpath );
    string_use( GLODWORD( mod_dir, FILE_PATH ) );

    /* Store the file name */
    if ( GLODWORD( mod_dir, FILE_NAME ) ) string_discard( GLODWORD( mod_dir, FILE_NAME ) );
    GLODWORD( mod_dir, FILE_NAME ) = string_new( data.cFileName );
    result = GLODWORD( mod_dir, FILE_NAME );
    string_use( result );  // 1 Por la variable de la estructura global

    /* Store integer and boolean variables */
    GLODWORD( mod_dir, FILE_DIRECTORY ) = (( data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) ? 1 : 0 );
    GLODWORD( mod_dir, FILE_HIDDEN ) = (( data.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN ) ? 1 : 0 );
    GLODWORD( mod_dir, FILE_READONLY ) = (( data.dwFileAttributes & FILE_ATTRIBUTE_READONLY ) ? 1 : 0 );
    GLODWORD( mod_dir, FILE_SIZE ) = data.nFileSizeLow;

    /* Format and store the creation time */
    FileTimeToSystemTime( &data.ftCreationTime, &time );
    sprintf( buffer, "%02d/%02d/%4d %02d:%02d", time.wDay, time.wMonth, time.wYear, time.wHour, time.wMinute );
    if ( GLODWORD( mod_dir, FILE_CREATED ) ) string_discard( GLODWORD( mod_dir, FILE_CREATED ) );
    GLODWORD( mod_dir, FILE_CREATED ) = string_new( buffer );
    string_use( GLODWORD( mod_dir, FILE_CREATED ) );

    /* Format and store the last write time */
    FileTimeToSystemTime( &data.ftLastWriteTime, &time );
    sprintf( buffer, "%02d/%02d/%4d %02d:%02d", time.wDay, time.wMonth, time.wYear, time.wHour, time.wMinute );
    if ( GLODWORD( mod_dir, FILE_MODIFIED ) ) string_discard( GLODWORD( mod_dir, FILE_MODIFIED ) );
    GLODWORD( mod_dir, FILE_MODIFIED ) = string_new( buffer );
    string_use( GLODWORD( mod_dir, FILE_MODIFIED ) );

    string_discard( params[ 0 ] );

    string_use( result );  // 1 Por el return
    return result;

#else

    const char * path = string_get( params[ 0 ] );
    char * path_final;
    const char * ptr;
    char * fptr;
    static char * last_path = NULL;
    static int currentFile = 0;
    static glob_t globd;
    struct stat s;
    int result;
    char buffer[ 128 ];
    char realpath[ __MAX_PATH ];

    /* Clean the path creating a case-insensitive match pattern */
    path_final = malloc( strlen( path ) * 4 );
    fptr = path_final;
    ptr = path;
    while ( *ptr )
    {
        if ( *ptr == '\\' )
            * fptr++ = '/';
        else if ( *ptr >= 'a' && *ptr <= 'z' )
        {
            *fptr++ = '[';
            *fptr++ = *ptr;
            *fptr++ = toupper( *ptr );
            *fptr++ = ']';
        }
        else if ( *ptr >= 'A' && *ptr <= 'Z' )
        {
            *fptr++ = '[';
            *fptr++ = tolower( *ptr );
            *fptr++ = *ptr;
            *fptr++ = ']';
        }
        else
            *fptr++ = *ptr;
        ptr++;
    }
    *fptr = 0;

    if ( !last_path || ( last_path && strcasecmp( last_path, path_final ) != 0 ) )
    {
        /* Using a different path */
        if ( last_path )
        {
            free( last_path );
            last_path = NULL;
            globfree( &globd );
        }

        last_path = strdup( path_final );

        // Convert *.* to *
        if ( fptr > path_final + 2 && fptr[ -1 ] == '*' && fptr[ -2 ] == '.' && fptr[ -3 ] == '*' )
            fptr[ -2 ] = 0;

#if defined(TARGET_MAC)
        glob( path_final, GLOB_ERR | GLOB_NOSORT, NULL, &globd );
#elif defined(TARGET_BEOS)
        glob( path_final, GLOB_ERR | GLOB_NOSORT, NULL, &globd );
#else
        glob( path_final, GLOB_ERR | GLOB_PERIOD | GLOB_NOSORT, NULL, &globd );
#endif
        currentFile = 0;
    }

    if ( currentFile == globd.gl_pathc )
    {
        /* Last file reached */
        if ( last_path ) free( last_path );
        last_path = NULL;
        string_discard( params[ 0 ] );
        free( path_final );
        result = string_new( "" );
        string_use( result );
        return ( result );
    }

    stat( globd.gl_pathv[ currentFile ], &s );

    /* Store the file name and path */
    if ( GLODWORD( mod_dir, FILE_NAME ) ) string_discard( GLODWORD( mod_dir, FILE_NAME ) );
    if ( GLODWORD( mod_dir, FILE_PATH ) ) string_discard( GLODWORD( mod_dir, FILE_PATH ) );

    ptr = strrchr( globd.gl_pathv[ currentFile ], '/' );
    if ( !ptr )
    {
        result = string_new( ptr = globd.gl_pathv[ currentFile ] );
        GLODWORD( mod_dir, FILE_NAME ) = result;
        string_use( result );

        GLODWORD( mod_dir, FILE_PATH ) = string_new( getcwd( realpath, sizeof( realpath ) ) );
        string_use( GLODWORD( mod_dir, FILE_PATH ) );
    }
    else
    {
        GLODWORD( mod_dir, FILE_NAME ) = result = string_new( ptr + 1 );
        string_use( GLODWORD( mod_dir, FILE_NAME ) );

        fptr = globd.gl_pathv[ currentFile ];
        GLODWORD( mod_dir, FILE_PATH ) = string_newa( fptr, ( ptr - fptr ) + 1 );
        string_use( GLODWORD( mod_dir, FILE_PATH ) );
        ptr++;
    }

    /* Store integer and boolean variables */
    GLODWORD( mod_dir, FILE_DIRECTORY ) = (( S_ISDIR( s.st_mode ) ) ? 1 : 0 );
    GLODWORD( mod_dir, FILE_HIDDEN ) = ( *ptr == '.' );
    GLODWORD( mod_dir, FILE_READONLY ) = !( s.st_mode & 0444 );
    GLODWORD( mod_dir, FILE_SIZE ) = s.st_size;

    /* Store file times */
    strftime( buffer, 100, "%d/%m/%Y %H:%M", localtime( &s.st_mtime ) );
    if ( GLODWORD( mod_dir, FILE_MODIFIED ) ) string_discard( GLODWORD( mod_dir, FILE_MODIFIED ) );
    GLODWORD( mod_dir, FILE_MODIFIED ) = string_new( buffer );
    string_use( GLODWORD( mod_dir, FILE_MODIFIED ) );

    strftime( buffer, 100, "%d/%m/%Y %H:%M", localtime( &s.st_ctime ) );
    if ( GLODWORD( mod_dir, FILE_CREATED ) ) string_discard( GLODWORD( mod_dir, FILE_CREATED ) );
    GLODWORD( mod_dir, FILE_CREATED ) = string_new( buffer );
    string_use( GLODWORD( mod_dir, FILE_CREATED ) );

    /* Return */
    currentFile++;
    string_use( result );
    string_discard( params[ 0 ] );
    free( path_final );
    return result;
#endif
}

/* ---------------------------------------------------------------------- */

DLSYSFUNCS __bgdexport( mod_dir, functions_exports)[] =
{
    /* Archivos y directorios */
    { "CD" , "" , TYPE_STRING , moddir_cd },
    { "CHDIR" , "S" , TYPE_INT , moddir_chdir },
    { "MKDIR" , "S" , TYPE_INT , moddir_mkdir },
    { "RMDIR" , "S" , TYPE_INT , moddir_rmdir },
    { "GLOB" , "S" , TYPE_STRING , moddir_glob },
    { "CD" , "S" , TYPE_STRING , moddir_chdir },
    { "RM" , "S" , TYPE_INT , moddir_rm },
    { 0 , 0 , 0 , 0 }
};

