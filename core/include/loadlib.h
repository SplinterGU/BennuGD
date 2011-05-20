/*
 *  Copyright © 2006-2011 SplinterGU (Fenix/Bennugd)
 *
 *  This file is part of Bennu - Game Development
 *
 *  This software is provided 'as-is', without any express or implied
 *  warranty. In no event will the authors be held liable for any damages
 *  arising from the use of this software.
 *
 *  Permission is granted to anyone to use this software for any purpose,
 *  including commercial applications, and to alter it and redistribute it
 *  freely, subject to the following restrictions:
 *
 *     1. The origin of this software must not be misrepresented; you must not
 *     claim that you wrote the original software. If you use this software
 *     in a product, an acknowledgment in the product documentation would be
 *     appreciated but is not required.
 *
 *     2. Altered source versions must be plainly marked as such, and must not be
 *     misrepresented as being the original software.
 *
 *     3. This notice may not be removed or altered from any source
 *     distribution.
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
    char *f;
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

    {
        dlibhandle * dlib = (dlibhandle*) malloc( sizeof( dlibhandle ) );
        if ( !dlib )
        {
            __dliberr = "Could not load library." ;
            dlclose( hnd );
            return NULL;
        }

        f = fname + strlen( fname );
        while ( f > fname && f[-1] != '\\' && f[-1] != '/' ) f-- ;
        dlib->fname = strdup( f );
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
}

static void * dlibaddr( dlibhandle * handle, const char * symbol )
{
    char * ptr, * f;

#ifdef _WIN32
    void * addr = GetProcAddress( (HMODULE)handle->hnd, symbol );
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

#ifndef TARGET_BEOS
    {
        Dl_info dli;
        dladdr( addr, &dli );

        ptr = ( char * ) dli.dli_fname; f = NULL;
        /*
            printf( "dli_fname=%s\n", dli.dli_fname );
            printf( "dli_fbase=%p\n", dli.dli_fbase );
            printf( "dli_sname=%s\n", dli.dli_sname );
            printf( "dli_saddr=%p\n", dli.dli_saddr );
        */
    }
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
#endif
/*
    printf( "[%s:%s]->%p\n", handle->fname, symbol, addr );fflush( stdout );
*/

    return addr;
}

static void * _dlibaddr( dlibhandle * handle, const char * symbol )
{
    char * ptr, * f;
    char * sym = (char*)malloc( strlen( handle->fname ) + strlen( symbol ) + 2 );
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

    {
        void * addr = dlibaddr( handle, sym );
        free( sym );
        return addr;
    }
}

#endif
