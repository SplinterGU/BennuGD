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
#include <ctype.h>

/* ------------------------------------------------------------------------------------ */
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
    /* Convert characters */
    while ( *p )
    {
        if ( *p == PATH_CHAR_ISEP ) *p = PATH_CHAR_SEP ;
        p++ ;
    }
    return c;
}


/* ------------------------------------------------------------------------------------ */
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
    return ( getcwd( NULL, 0 ) ) ;
}

/* ------------------------------------------------------------------------------------ */
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

/* ------------------------------------------------------------------------------------ */
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

/* ------------------------------------------------------------------------------------ */
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

/* ------------------------------------------------------------------------------------ */
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

/* ------------------------------------------------------------------------------------ */

__DIR_ST * dir_open( const char * path )
{
    __DIR_ST * hDir = malloc( sizeof( __DIR_ST ) );

    hDir->path = strdup( path );

#ifdef _WIN32
    hDir->handle = FindFirstFile( hDir->path, &hDir->data );
    hDir->eod = ( hDir->handle == INVALID_HANDLE_VALUE ) ;
#else
    char * path_final = malloc( strlen( path ) * 4 );
    const char * ptr = hDir->path ;
    char * fptr = path_final ;

    /* Clean the path creating a case-insensitive match pattern */

    while ( *ptr )
    {
        if ( *ptr == '\\' )
        {
            *fptr++ = '/';
        }
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

    /* Convert *.* to * */
    if ( fptr > path_final + 2 && fptr[ -1 ] == '*' && fptr[ -2 ] == '.' && fptr[ -3 ] == '*' ) fptr[ -2 ] = 0;

#if defined(TARGET_MAC)
    glob( path_final, GLOB_ERR | GLOB_NOSORT, NULL, &hDir->globd );
#elif defined(TARGET_BEOS)
    glob( path_final, GLOB_ERR | GLOB_NOSORT, NULL, &hDir->globd );
#else
    glob( path_final, GLOB_ERR | GLOB_PERIOD | GLOB_NOSORT, NULL, &hDir->globd );
#endif

    hDir->currFile = 0;
#endif

    return hDir;
}

/* ------------------------------------------------------------------------------------ */

void dir_close ( __DIR_ST * hDir )
{
    free ( hDir->path );

#ifdef _WIN32
    FindClose( hDir->handle );
#else
    globfree( &hDir->globd );
#endif

    free ( hDir );
}

/* ------------------------------------------------------------------------------------ */

__DIR_FILEINFO_ST * dir_read( __DIR_ST * hDir )
{
    char realpath[__MAX_PATH];
    char * ptr ;

#ifdef _WIN32
    SYSTEMTIME time;

    if ( hDir->eod ) return NULL;

    /* Fill the FILEINFO struct */
    strcpy( realpath, hDir->path );
    ptr = realpath + strlen( realpath );
    while ( ptr >= realpath )
    {
        if ( *ptr == '\\' || *ptr == '/' )
        {
            ptr[ 1 ] = 0;
            break;
        }
        ptr--;
    }

    strcat( realpath, hDir->data.cFileName );
    GetFullPathName( realpath, __MAX_PATH, hDir->info.fullpath, &ptr );
    if ( ptr ) * ptr = '\0';

    strcpy ( hDir->info.filename, hDir->data.cFileName );

    hDir->info.attributes    = (( hDir->data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) ? DIR_FI_ATTR_DIRECTORY : 0 ) |
                               (( hDir->data.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN    ) ? DIR_FI_ATTR_HIDDEN    : 0 ) |
                               (( hDir->data.dwFileAttributes & FILE_ATTRIBUTE_READONLY  ) ? DIR_FI_ATTR_READONLY  : 0 );

    hDir->info.size          = hDir->data.nFileSizeLow;

    /* Format and store the creation time */
    FileTimeToSystemTime( &hDir->data.ftCreationTime, &time );

    hDir->info.creation_time.tm_sec      = time.wSecond;
    hDir->info.creation_time.tm_min      = time.wMinute;
    hDir->info.creation_time.tm_hour     = time.wHour;
    hDir->info.creation_time.tm_mday     = time.wDay;
    hDir->info.creation_time.tm_mon      = time.wMonth - 1;
    hDir->info.creation_time.tm_year     = time.wYear - 1900;
    hDir->info.creation_time.tm_wday     = time.wDayOfWeek;
    hDir->info.creation_time.tm_yday     = time.wMonth;
    hDir->info.creation_time.tm_isdst    = -1;

    /* Format and store the last write time */
    FileTimeToSystemTime( &hDir->data.ftLastWriteTime, &time );

    hDir->info.modified_time.tm_sec      = time.wSecond;
    hDir->info.modified_time.tm_min      = time.wMinute;
    hDir->info.modified_time.tm_hour     = time.wHour;
    hDir->info.modified_time.tm_mday     = time.wDay;
    hDir->info.modified_time.tm_mon      = time.wMonth - 1;
    hDir->info.modified_time.tm_year     = time.wYear - 1900;
    hDir->info.modified_time.tm_wday     = time.wDayOfWeek;
    hDir->info.modified_time.tm_yday     = time.wMonth;
    hDir->info.modified_time.tm_isdst    = -1;

    /* Continue last search */
    if (!FindNextFile( hDir->handle, &hDir->data )) hDir->eod = 1;
#else
    struct stat s;

    if ( hDir->currFile == hDir->globd.gl_pathc ) return NULL;

    stat( hDir->globd.gl_pathv[ hDir->currFile ], &s );

    ptr = strrchr( hDir->globd.gl_pathv[ hDir->currFile ], '/' );
    if ( !ptr )
    {
        strcpy ( hDir->info.filename, hDir->globd.gl_pathv[ hDir->currFile ] );
        strcpy ( hDir->info.fullpath, getcwd( realpath, sizeof( realpath ) ) );
    }
    else
    {
        strcpy ( hDir->info.filename, ptr + 1 );
        if ( hDir->globd.gl_pathv[ hDir->currFile ][0] == '/' )
        {
            strcpy ( hDir->info.fullpath, hDir->globd.gl_pathv[ hDir->currFile ] );
            hDir->info.fullpath[ ptr - hDir->globd.gl_pathv[ hDir->currFile ] + 1 ] = '\0';
        }
        else
        {
            strcpy ( hDir->info.fullpath, getcwd( realpath, sizeof( realpath ) ) );
            strcat ( hDir->info.fullpath, "/" );
            strcat ( hDir->info.fullpath, hDir->globd.gl_pathv[ hDir->currFile ] );
            ptr = strrchr( hDir->info.fullpath, '/' );
            *(ptr + 1) = '\0';
        }
    }

    hDir->info.attributes    = (( S_ISDIR( s.st_mode )          ) ? DIR_FI_ATTR_DIRECTORY : 0 ) |
                               (( hDir->info.filename[0] == '.' ) ? DIR_FI_ATTR_HIDDEN    : 0 ) |
                               (( !( s.st_mode & 0444 )         ) ? DIR_FI_ATTR_READONLY  : 0 );

    hDir->info.size          = s.st_size;

    hDir->info.creation_time = *localtime( &s.st_ctime ) ;
    hDir->info.modified_time = *localtime( &s.st_mtime ) ;

    hDir->currFile++;

#endif

    return ( &hDir->info );
}

/* ------------------------------------------------------------------------------------ */
