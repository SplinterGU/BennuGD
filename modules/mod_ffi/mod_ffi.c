/*
 *  Copyright © 2006-2010 SplinterGU (Fenix/Bennugd)
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

#include <loadlib.h> /* Must be first include */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ffi.h>

#include "bgddl.h"
#include "xstrings.h"

/* --------------------------------------------------------------------------- */

typedef struct
{
    void * lib;
    void * pfunc;
    ffi_cif * cif;
    char rettype;
    char * argstype;
} modffi_handle;

/* ---------------------------------------------------------------------- */

static int modffi_declare( INSTANCE * my, int * params )
{
    modffi_handle * hnd;
    char    * pargs = string_get( params[3] ),
            * prettype = string_get( params[4] );
    ffi_type ** argstype = NULL, * rettype = NULL;
    int i, len;

    hnd = malloc( sizeof( modffi_handle ) );
    if ( !hnd )
    {
        string_discard( params[4] ) ;
        string_discard( params[3] ) ;
        string_discard( params[2] ) ;
        string_discard( params[1] ) ;

        return 0;
    }

    hnd->lib  = dlibopen( string_get( params[1] ) ) ;
    if ( !hnd->lib )
    {
        string_discard( params[4] ) ;
        string_discard( params[3] ) ;
        string_discard( params[2] ) ;
        string_discard( params[1] ) ;

        free( hnd );
        return 0;
    }

    hnd->pfunc = dlibaddr( hnd->lib, string_get( params[2] ) ) ;
    if ( !hnd->pfunc )
    {
        string_discard( params[4] ) ;
        string_discard( params[3] ) ;
        string_discard( params[2] ) ;
        string_discard( params[1] ) ;

        dlibclose( hnd->lib );
        free( hnd );
        return 0;
    }

    hnd->cif = malloc( sizeof( ffi_cif ) );
    if ( !hnd->cif )
    {
        string_discard( params[4] ) ;
        string_discard( params[3] ) ;
        string_discard( params[2] ) ;
        string_discard( params[1] ) ;

        dlibclose( hnd->lib );
        free( hnd );
        return 0;
    }

    len = strlen( pargs );
    argstype = calloc( len, sizeof( ffi_type * ) );
    if ( !argstype )
    {
        string_discard( params[4] ) ;
        string_discard( params[3] ) ;
        string_discard( params[2] ) ;
        string_discard( params[1] ) ;

        dlibclose( hnd->lib );
        free( hnd->cif );
        free( hnd );
        return 0;
    }


    if ( prettype )
        hnd->rettype = * prettype;
    else
        hnd->rettype = '\0';

    hnd->argstype = strdup( pargs );
    if ( !hnd->argstype )
    {
        string_discard( params[4] ) ;
        string_discard( params[3] ) ;
        string_discard( params[2] ) ;
        string_discard( params[1] ) ;

        dlibclose( hnd->lib );
        free( argstype );
        free( hnd->cif );
        free( hnd );
        return 0;
    }

    for ( i = 0; i < len; i++ )
    {
        switch ( pargs[i] )
        {
            case 'I':
                argstype[i] = &ffi_type_uint32;
                break;

            case 'W':
                argstype[i] = &ffi_type_uint16;
                break;

            case 'B':
                argstype[i] = &ffi_type_sint8;
                break;

            case 'F':
                argstype[i] = &ffi_type_float;
                break;
                /*
                case 'S':
                    argstype[i] = &ffi_type_pointer;
                    break;
                */
            case 'P':
                argstype[i] = &ffi_type_pointer;
                break;

            default:
                string_discard( params[4] ) ;
                string_discard( params[3] ) ;
                string_discard( params[2] ) ;
                string_discard( params[1] ) ;

                dlibclose( hnd->lib );
                free( hnd->argstype );
                free( argstype );
                free( hnd->cif );
                free( hnd );
                return 0;
        }
    }

    if ( hnd->rettype )
    {
        switch ( hnd->rettype )
        {
            case 'I':
                rettype = &ffi_type_uint32;
                break;

            case 'W':
                rettype = &ffi_type_uint16;
                break;

            case 'B':
                rettype = &ffi_type_sint8;
                break;

            case 'F':
                rettype = &ffi_type_float;
                break;
                /*
                case 'S':
                    rettype = &ffi_type_pointer;
                    break;
                */
            case 'P':
                rettype = &ffi_type_pointer;
                break;

            default:
                string_discard( params[4] ) ;
                string_discard( params[3] ) ;
                string_discard( params[2] ) ;
                string_discard( params[1] ) ;

                dlibclose( hnd->lib );
                free( hnd->argstype );
                free( argstype );
                free( hnd->cif );
                free( hnd );
                return 0;
        }
    }

    string_discard( params[4] ) ;
    string_discard( params[3] ) ;
    string_discard( params[2] ) ;
    string_discard( params[1] ) ;

    /* Initialize the cif */
    if ( ffi_prep_cif( hnd->cif, params[0], len, rettype, argstype ) == FFI_OK ) return hnd;

    dlibclose( hnd->lib );
    free( hnd->argstype );
    free( argstype );
    free( hnd->cif );
    free( hnd );

    return 0 ;
}

/* ---------------------------------------------------------------------- */

static int modffi_call( INSTANCE * my, int * params )
{
    modffi_handle * hnd = params[0];
    ffi_call( hnd->cif, FFI_FN( hnd->pfunc ), params[1], params[2] );
    return 0;
}

/* --------------------------------------------------------------------------- */

DLCONSTANT  __bgdexport( mod_ffi, constants_def )[] =
{
    { "FFI_FIRST_ABI"       , TYPE_DWORD    , FFI_FIRST_ABI     },

    /* ---- Intel x86 Win32 ---------- */
#ifdef _WIN32
    { "FFI_SYSV"            , TYPE_DWORD    , FFI_SYSV          },
    { "FFI_STDCALL"         , TYPE_DWORD    , FFI_STDCALL       },
    /* TODO: Add fastcall support for the sake of completeness */
#endif

    /* ---- Intel x86 and AMD x86-64 - */
#if !defined(_WIN32) && (defined(__i386__) || defined(__x86_64__))
    { "FFI_SYSV"            , TYPE_DWORD    , FFI_SYSV          },
    { "FFI_UNIX64"          , TYPE_DWORD    , FFI_UNIX64        },      /* Unix variants all use the same ABI for x86-64  */
#endif

    { "FFI_DEFAULT_ABI"     , TYPE_DWORD    , FFI_DEFAULT_ABI   },
    { "FFI_LAST_ABI"        , TYPE_DWORD    , FFI_LAST_ABI      },

    { NULL                  , 0             , 0                 }
} ;

/* ----------------------------------------------------------------- */
/* Declaracion de funciones                                          */

DLSYSFUNCS  __bgdexport( mod_ffi, functions_exports )[] =
{
    { "FFI_DECLARE" , "ISSSS"   , TYPE_INT  , modffi_declare    },
    { "FFI_CALL"    , "IPP"     , TYPE_INT  , modffi_call       },
    { 0             , 0         , 0         , 0                 }
};

/* ----------------------------------------------------------------- */



