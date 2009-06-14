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

#ifdef WIN32
/* NEEDED FOR LOCALE DETECTION */
#include <windows.h>
#include <windef.h>
#include <winnls.h>
#include "Shlwapi.h"
#endif

#include "bgdc.h"

#include "errors.h"

/* --------------------------------------------------------------------------- */

#define REMOVE_EXT(s) {char *p; if ((p = strrchr(s, '.')) && (p > strrchr(s, '\\') && p > strrchr(s, '/'))) * p = '\0';}

extern int load_file( char * filename );
extern void import_files( char * filename );
extern void add_simple_define( char * macro, char *text );
extern int dcb_options;

/* --------------------------------------------------------------------------- */

char langinfo[64];

int debug = 0;
int autodeclare = 1;

char * main_path = NULL;

/* --------------------------------------------------------------------------- */

static char timebuff[11]; /* YYYY/MM/DD or HH:MM:SS */

/* --------------------------------------------------------------------------- */

int main( int argc, char **argv )
{
    char * sourcefile = 0;
    char basepathname[__MAX_PATH] = "";
    char dcbname[__MAX_PATH] = "";
    char stubname[__MAX_PATH] = "";
    char importname[__MAX_PATH] = "";
    char compilerimport[__MAX_PATH] = "";
    int i, j;

    printf( BGDC_VERSION "\n"
            "Copyright © 2006-2009 SplinterGU (Fenix/BennuGD)\n"
            "Copyright © 2002-2006 Fenix Team (Fenix)\n"
            "Copyright © 1999-2002 José Luis Cebrián Pagüe (Fenix)\n"
            "Bennu Game Development comes with ABSOLUTELY NO WARRANTY;\n"
            "see COPYING for details\n\n" );

    /* Default lang to EN */
    strcpy( langinfo, "EN" );
    /* LANG detect */
#ifdef WIN32
    GetLocaleInfo( LOCALE_USER_DEFAULT, LOCALE_SABBREVCTRYNAME, langinfo, 64 );
    strlwr( langinfo );
#else
    if ( getenv( "LANG" ) != NULL && strlen( getenv( "LANG" ) ) >= 2 )
        strcpy( langinfo, getenv( "LANG" ) );
#endif
    langinfo[2] = 0;

    srand( time( NULL ) );

    /* build error messages list */
    err_buildErrorTable();

    init_c_type();
    identifier_init();
    constants_init();
    string_init();
    compile_init();

    /* Get command line parameters */

    for ( i = 1 ; i < argc ; i++ )
    {
        if ( argv[i][0] == '-' )
        {
            if ( !strcmp( argv[i], "--pedantic" ) )
            {
                autodeclare = 0 ;
                continue;
            }

            j = 1;
            while ( argv[i][j] )
            {
                if ( argv[i][j] == 'd' )
                {
                    if ( argv[i][j + 1] >= '0' && argv[i][j + 1] <= '9' )
                    {
                        debug = atoi( &argv[i][j + 1] );
                    }
                    else
                    {
                        debug = 1;
                    }
                }

                if ( argv[i][j] == 'o' )
                {
                    if ( argv[i][j + 1] )
                        strncpy( dcbname, &argv[i][j + 1], sizeof( dcbname ) );
                    else if ( argv[i + 1] && argv[i + 1][0] != '-' )
                        strncpy( dcbname, argv[++i], sizeof( dcbname ) );
                    break;
                }

                if ( argv[i][j] == 'c' ) dos_chars = 1;

                if ( argv[i][j] == 'a' ) autoinclude = 1;

                if ( argv[i][j] == 'g' ) dcb_options |= DCB_DEBUG;

                if ( argv[i][j] == 'p' ) autodeclare = 1 ;

                if ( argv[i][j] == 's' )
                {
                    /* -s "stub": Use a stub */

                    if ( argv[i][j + 1] )
                        strncpy( stubname, &argv[i][j + 1], __MAX_PATH );
                    else if ( argv[i + 1] && argv[i + 1][0] != '-' )
                        strncpy( stubname, argv[++i], __MAX_PATH );
                    break;
                }

                if ( argv[i][j] == 'f' )
                {
                    /* -f "file": Embed a file to the DCB */

                    if ( argv[i][j + 1] )
                        dcb_add_file( argv[i + j] + 1 );
                    else while ( argv[i + 1] )
                        {
                            if ( argv[i + 1][0] == '-' ) break;
                            dcb_add_file( argv[i + 1] );
                            i++;
                        }
                    break;
                }

                if ( argv[i][j] == 'i' )
                {
                    /* -i "path": add a file to the path for include files */

                    if ( argv[i][j + 1] == 0 )
                    {
                        if ( i == argc - 1 )
                        {
                            printf( MSG_DIRECTORY_MISSING "\n" );
                            exit( 1 );
                        }
                        file_addp( argv[i + 1] );
                        i++;
                        break;
                    }
                    file_addp( argv[i] + j + 1 );
                    break;
                }

                if ( argv[i][j] == 'l' )
                {
                    /* -lLANG:  Set the language for errors and messages */

                    if ( argv[i][j + 1] == 0 )
                    {
                        if ( i != argc - 1 )
                        {
                            strcpy( langinfo, argv[i + 1] );
                        }
                        i++;
                        break;
                    }
                    strcpy( langinfo, argv[i] + j + 1 );
                    break;
                }

                if ( argv[i][j] == 'D' )
                {
                    char * macro = NULL ;
                    char * text = NULL ;

                    /* -D<macro>=<text> */

                    if ( argv[i][j + 1] )
                    {
                        macro = strdup( &argv[i][j + 1] );
                    }
                    else
                    {
                        if ( argv[i + 1][0] == '-' ) break;
                        macro = strdup( argv[i + 1] );
                        i++;
                    }

                    if (( text = strchr( macro, '=' ) ) )
                    {
                        * text = '\0';
                        text++;
                    }
                    else
                    {
                        text = "";
                    }

                    add_simple_define( macro, text );
                    free( macro );
                    break;
                }

                if ( argv[i][j] == 'C' )
                {
                    if ( argv[i][j + 1] == 'a' ) autodeclare = 1 ;
                    break;
                }

                j++;
            }
        }
        else
        {
            if ( sourcefile )
            {
                printf( MSG_TOO_MANY_FILES "\n" );
                return 0;
            }

            char * p, * pathend = NULL;

            sourcefile = argv[i];
            p = main_path = strdup( argv[i] );
            while ( p && *p )
            {
                if ( *p == ':' || *p == '\\' || *p == '/' ) pathend = p;
                p++;
            }
            if ( pathend )
            {
                *( pathend + 1 ) = '\0';
                file_addp( main_path );
            }
            else
            {
                free( main_path );
                main_path = getcwd(malloc(__MAX_PATH), __MAX_PATH);
                strcat(main_path, PATH_SEP);
            }
        }
    }

    if ( !sourcefile )
    {
        printf( MSG_USING
                MSG_OPTION_D
                MSG_OPTIONS
                MSG_LICENSE, argv[0] );
        return 0;
    }

    add_simple_define( "COMPILER_VERSION", VERSION );
    add_simple_define( "__VERSION__", VERSION );

    {
        time_t curtime;
        struct tm *loctime;
        int value, code;

        curtime = time( NULL ); /* Get the current time. */
        loctime = localtime( &curtime ); /* Convert it to local time representation. */

        strftime( timebuff, sizeof( timebuff ), "%Y/%m/%d", loctime );
        value = string_new( timebuff );
        code = identifier_search_or_add( "__DATE__" ) ;
        constants_add( code, typedef_new( TYPE_STRING ), value ) ;

        strftime( timebuff, sizeof( timebuff ), "%H:%M:%S", loctime );
        value = string_new( timebuff );
        code = identifier_search_or_add( "__TIME__" ) ;
        constants_add( code, typedef_new( TYPE_STRING ), value ) ;
    }

    memset( &dcb, 0, sizeof( dcb ) );

    core_init();
    sysproc_init();

    /* Files names */

    strcpy( basepathname, sourcefile );
    REMOVE_EXT( basepathname );

    /* Default compiler imports */
    strcpy( compilerimport, argv[0] );
#ifdef WIN32
    REMOVE_EXT( compilerimport );
#endif
    strcat( compilerimport, ".imp" );
    import_files( compilerimport );
    strcat( compilerimport, "ort" ); /* name.import */
    import_files( compilerimport );

    /* Project imports */
    strcpy( importname, basepathname ); strcat( importname, ".imp" );
    import_files( importname );

    strcat( importname, "ort" ); /* name.import */
    import_files( importname );

    /* Load Main Source File */
    load_file( sourcefile );

    compile_program();

    if ( !dcbname[0] )
    {
        strcpy( dcbname, basepathname ); strcat( dcbname, ".dcb" );
    }

    if ( stubname[0] != 0 )
    {
        if ( !file_exists( stubname ) )
        {
#ifdef WIN32
            char exepath[__MAX_PATH];

            GetModuleFileName( NULL, exepath, sizeof( exepath ) );
            PathRemoveFileSpec( exepath );
            strcat( exepath, "\\" );
            memmove( stubname + strlen( exepath ), stubname, strlen( stubname ) + 1 );
            memcpy( stubname, exepath, strlen( exepath ) );
#else
            const char * ptr = argv[0] + strlen( argv[0] );
            while ( ptr > argv[0] && *ptr != '\\' && *ptr != '/' ) ptr--;
            if ( *ptr == '\\' || *ptr == '/' ) ptr++;
            if ( ptr > argv[0] )
            {
                memmove( stubname + ( ptr - argv[0] ), stubname, strlen( stubname ) + 1 );
                memcpy( stubname, argv[0], ptr - argv[0] );
            }
#endif
            if ( !file_exists( stubname ) )
            {
#ifdef WIN32
                strcat( stubname, ".exe" );
                if ( !file_exists( stubname ) )
                {
#endif
                    compile_error( "Can't open stub file %s", stubname );
#ifdef WIN32
                    return -1;
                }
#endif

            }
        }

        REMOVE_EXT( dcbname );
#ifdef WIN32
        strcat( dcbname, ".exe" );
#endif
        dcb_save( dcbname, dcb_options, stubname );
    }
    else
    {
        dcb_save( dcbname, dcb_options, NULL );
    }

    /* destroy error messages list */
    err_destroyErrorTable();

    return 1;
}

/* --------------------------------------------------------------------------- */
