/*
 *  Copyright © 2006-2011 SplinterGU (Fenix/Bennugd)
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
 * INCLUDES
 */

#ifdef _WIN32
#define  _WIN32_WINNT 0x0500
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "bgdi.h"
#include "bgdrtm.h"
#include "xstrings.h"
#include "dirs.h"

/* ---------------------------------------------------------------------- */

static char * dcb_exts[] = { ".dcb", ".dat", ".bin", NULL };

static int standalone  = 0;  /* 1 only if this is an standalone interpreter   */
static int embedded    = 0;  /* 1 only if this is a stub with an embedded DCB */

/* ---------------------------------------------------------------------- */

/*
 *  FUNCTION : main
 *
 *  Main function
 *
 *  PARAMS:
 *      INT n: ERROR LEVEL to return to OS
 *
 *  RETURN VALUE:
 *      No value
 *
 */

int main( int argc, char *argv[] )
{
    char * filename = NULL, dcbname[ __MAX_PATH ], *ptr ;
    int i, j, ret = -1;
    file * fp = NULL;
    INSTANCE * mainproc_running;
    dcb_signature dcb_signature;

    /* get my executable name */
    ptr = argv[0] + strlen( argv[0] );
    while ( ptr > argv[0] && ptr[-1] != '\\' && ptr[-1] != '/' ) ptr-- ;
    appexename = strdup( ptr );

    /* get executable full pathname  */
    appexefullpath = getfullpath( argv[0] );
    if ( ( !strchr( argv[0], '\\' ) && !strchr( argv[0], '/' ) ) && !file_exists( appexefullpath ) )
    {
        char *p = whereis( appexename );
        if ( p )
        {
            char * tmp = calloc( 1, strlen( p ) + strlen( appexename ) + 2 );
            free( appexefullpath );
            sprintf( tmp, "%s/%s", p, appexename );
            appexefullpath = getfullpath( tmp );
            free( tmp );
        }
    }

    /* get pathname of executable */
    ptr = strstr( appexefullpath, appexename );
    appexepath = calloc( 1, ptr - appexefullpath + 1 );
    strncpy( appexepath, appexefullpath, ptr - appexefullpath );

#ifdef _WIN32
    appname = calloc( 1, strlen( appexename ) - 3 );
    strncpy( appname, appexename, strlen( appexename ) - 4 );
#else
    appname = strdup( appexename );
#endif

    standalone = ( strncmpi( appexename, "bgdi", 4 ) == 0 ) ;

    /* add binary path */
    file_addp( appexepath );

    if ( !standalone )
    {
        /* Hand-made interpreter: search for DCB at EOF */
        fp = file_open( argv[0], "rb0" );
        if ( fp )
        {
            file_seek( fp, -( int )sizeof( dcb_signature ), SEEK_END );
            file_read( fp, &dcb_signature, sizeof( dcb_signature ) );

            if ( strcmp( dcb_signature.magic, DCB_STUB_MAGIC ) == 0 )
            {
                ARRANGE_DWORD( &dcb_signature.dcb_offset );
                embedded = 1;
            }
        }

        filename = appname;
    }

    if ( standalone )
    {
        /* Calling BGDI.EXE so we must get all command line params */

        for ( i = 1 ; i < argc ; i++ )
        {
            if ( argv[i][0] == '-' )
            {
                j = 1 ;
                while ( argv[i][j] )
                {
                    if ( argv[i][j] == 'd' ) debug = 1 ;
                    if ( argv[i][j] == 'i' )
                    {
                        if ( argv[i][j+1] == 0 )
                        {
                            if ( i == argc - 1 )
                            {
                                fprintf( stderr, "You must provide a directory" ) ;
                                exit( 0 );
                            }
                            file_addp( argv[i+1] );
                            i++ ;
                            break ;
                        }
                        file_addp( argv[i] + j + 1 ) ;
                        break ;
                    }
                    j++ ;
                }
            }
            else
            {
                if ( !filename )
                {
                    filename = argv[i] ;
                    if ( i < argc - 1 ) memmove( &argv[i], &argv[i+1], sizeof( char* ) * ( argc - i - 1 ) ) ;
                    argc-- ;
                    i-- ;
                }
            }
        }

        if ( !filename )
        {
            printf( BGDI_VERSION "\n"
                    "Copyright (c) 2006-2011 SplinterGU (Fenix/BennuGD)\n"
                    "Copyright (c) 2002-2006 Fenix Team (Fenix)\n"
                    "Copyright (c) 1999-2002 José Luis Cebrián Pagüe (Fenix)\n"
                    "Bennu Game Development comes with ABSOLUTELY NO WARRANTY;\n"
                    "see COPYING for details\n\n"
                    "Usage: %s [options] <data code block file>[.dcb]\n\n"
                    "   -d       Activate DEBUG mode\n"
                    "   -i dir   Adds the directory to the PATH\n"
                    "\nThis program is free software distributed under.\n\n"
                    "GNU General Public License published by Free Software Foundation.\n"
                    "Permission granted to distribute and/or modify as stated in the license\n"
                    "agreement (GNU GPL version 2 or later).\n"
                    "See COPYING for license details.\n",
                    argv[0] ) ;
            return -1 ;
        }
    }

    /* Initialization (modules needed before dcb_load) */

    string_init() ;
    init_c_type() ;

    /* Init application title for windowed modes */

    strcpy( dcbname, filename ) ;

    if ( appname && filename != appname )
    {
        free( appname );
        appname = strdup( filename ) ;
    }

    if ( !embedded )
    {
        /* First try to load directly (we expect myfile.dcb) */
        if ( !dcb_load( dcbname ) )
        {
            char ** dcbext = dcb_exts;
            int dcbloaded = 0;

            while ( dcbext && *dcbext )
            {
                strcpy( dcbname, filename ) ;
                strcat( dcbname, *dcbext ) ;
                if (( dcbloaded = dcb_load( dcbname ) ) ) break;
                dcbext++;
            }

            if ( !dcbloaded )
            {
                printf( "%s: doesn't exist or isn't version %d DCB compatible\n", filename, DCB_VERSION >> 8 ) ;
                return -1 ;
            }
        }
    }
    else
    {
        dcb_load_from( fp, dcb_signature.dcb_offset );
    }

    /* If the dcb is not in debug mode */

    if ( dcb.data.NSourceFiles == 0 ) debug = 0;

    /* Initialization (modules needed after dcb_load) */

    sysproc_init() ;

#ifdef _WIN32
    HWND hWnd = /*GetForegroundWindow()*/ GetConsoleWindow();
    DWORD dwProcessId;
    GetWindowThreadProcessId( hWnd, &dwProcessId );
    if ( dwProcessId == GetCurrentProcessId() ) ShowWindow( hWnd, SW_HIDE );
#endif

    argv[0] = filename;
    bgdrtm_entry( argc, argv );

    if ( mainproc )
    {
        mainproc_running = instance_new( mainproc, NULL ) ;
        ret = instance_go_all() ;
    }

    bgdrtm_exit( ret );

    free( appexename        );
    free( appexepath        );
    free( appexefullpath    );
    free( appname           );

    return ret;
}

