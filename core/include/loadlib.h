/*
 *  Copyright � 2006-2008 SplinterGU (Fenix/Bennugd)
 *  Copyright � 2002-2006 Fenix Team (Fenix)
 *  Copyright � 1999-2002 Jos� Luis Cebri�n Pag�e (Fenix)
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

#ifndef _LOADLIB_H
#define _LOADLIB_H

#ifdef _WIN32
#include <windows.h>
#include <winbase.h>
#else
#define _GNU_SOURCE
#include <dlfcn.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define __stdcall
#define __dllexport
#define __dllimport
#endif

#ifdef _WIN32
#define dlclose(a)
#endif

typedef struct
{
    char * fname;
    void * hnd;
} dlibhandle ;

static char * __dliberr = NULL;

static char * dliberror( void )
{
    return __dliberr;
}

static int dlibclose( dlibhandle * handle )
{
    dlclose( handle->hnd );
    free( handle->fname );
    free( handle );

    return 0;
}

static dlibhandle * dlibopen( const char * fname )
{
#ifdef _WIN32
    void * hnd = LoadLibrary( fname );
#else
    void * hnd = dlopen( fname, RTLD_NOW | RTLD_GLOBAL );
#endif
    if ( !hnd )
    {
#ifdef _WIN32
        __dliberr = "Could not load library." ;
#else
        __dliberr = dlerror() ;
#endif
        return NULL;
    }
    dlibhandle * dlib = malloc( sizeof( dlibhandle ) );
    if ( !dlib )
    {
        __dliberr = "Could not load library." ;
        dlclose( hnd );
        return NULL;
    }

    dlib->fname = strdup( fname );
    if ( !dlib->fname )
    {
        __dliberr = "Could not load library." ;
        free( dlib );
        dlclose( hnd );
        return NULL;
    }
    dlib->hnd = hnd;

    return ( dlib );
}

static void * dlibaddr( dlibhandle * handle, const char * symbol )
{
    char * ptr, * f;

#ifdef _WIN32
    void * addr = GetProcAddress( handle->hnd, symbol );
    if ( !addr )
    {
        __dliberr = "Symbol not found." ;
        return NULL;
    }
#else
    void * addr = dlsym( handle->hnd, symbol ) ;

    if ( !addr )
    {
        __dliberr = dlerror() ;
        return NULL;
    }

    Dl_info dli;
    dladdr( addr, &dli );

    /*
        printf( "dli_fname=%s\n", dli.dli_fname );
        printf( "dli_fbase=%p\n", dli.dli_fbase );
        printf( "dli_sname=%s\n", dli.dli_sname );
        printf( "dli_saddr=%p\n", dli.dli_saddr );
    */

    ptr = ( char * ) dli.dli_fname; f = NULL;
    while ( *ptr )
    {
        if ( *ptr == '/' || *ptr == '\\' ) f = ptr ;
        ptr++;
    }
    if ( f ) ptr = f + 1;

    if ( strcmp( ptr, handle->fname ) )
    {
        __dliberr = "Symbol not found." ;
        return NULL;
    }
#endif
//    printf( "[%s:%s]->%p\n", handle->fname, symbol, addr );fflush( stdout );

    return addr;
}

static void * _dlibaddr( dlibhandle * handle, const char * symbol )
{
    char * ptr, * f;
    char * sym = malloc( strlen( handle->fname ) + strlen( symbol ) + 2 );
    if ( !sym )
    {
        __dliberr = "Can't load symbol." ;
        return NULL;
    }

    strcpy( sym, handle->fname );
    ptr = ( char * ) sym; f = NULL;
    while ( *ptr )
    {
        if ( *ptr == '.' ) f = ptr ;
        ptr++;
    }
    if ( f ) *f = '\0';
    strcat( sym, "_" ); strcat( sym, symbol );

    void * addr = dlibaddr( handle, sym );

    free( sym );

    return addr;
}

#endif