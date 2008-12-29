/*
 *  Copyright © 2008 SplinterGU
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

#include <loadlib.h> /* Must be fist include */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>

#include <bgddl.h>
#include <dlvaracc.h>
#include <files_st.h>

/* ----------------------------------------------------------------- */

int onlyfenixexport = 0;
int interactive = 1;

/* ----------------------------------------------------------------- */

void describe_type( int type )
{
    switch ( type )
    {
        case TYPE_DWORD:
            printf( "DWORD" ) ;
            break ;
        case TYPE_INT:
            printf( "INT" ) ;
            break ;
        case TYPE_SHORT:
            printf( "SHORT" ) ;
            break ;
        case TYPE_WORD:
            printf( "WORD" ) ;
            break ;
        case TYPE_BYTE:
            printf( "BYTE" ) ;
            break ;
        case TYPE_SBYTE:
            printf( "SIGNED BYTE" ) ;
            break ;
        case TYPE_CHAR:
            printf( "CHAR" ) ;
            break ;
        case TYPE_STRING:
            printf( "STRING" ) ;
            break ;
        case TYPE_FLOAT:
            printf( "FLOAT" ) ;
            break ;
        case TYPE_POINTER:
            printf( "POINTER" ) ;
            break ;
        case TYPE_UNDEFINED:
            printf( "UNDEFINED" );
            break ;
    }
}

/* ----------------------------------------------------------------- */

void * fnc_import( const char * name )
{
    if ( !onlyfenixexport ) printf( "Import %s\n", name );
    return NULL;
}

/* ----------------------------------------------------------------- */

int describe_func( char * name, char * paramtypes, int type, void * func )
{
    int param;
    if ( func ) printf( "Export " );
    describe_type( type );
    printf( " %s(", name );
    param = 0;
    while ( paramtypes && paramtypes[param] )
    {
        switch ( paramtypes[param] )
        {
            case 'I':
                printf( "INTEGER" );
                break;

            case 'B':
                printf( "BYTE" );
                break;

            case 'W':
                printf( "WORD" );
                break;

            case 'S':
                printf( "STRING" );
                break;

            case 'P':
                printf( "POINTER" );
                break;

            case 'F':
                printf( "FLOAT" );
                break;

            case 'V':
                printf( "VARIABLE" );
                param += 2;
                break;

            default:
                printf( "[PARAM ERROR]" ) ;
        }

        if ( paramtypes[param+1] ) printf( ", " );
        param++;
    }
    printf( ")\n" );

    return 0 ;
}

/* ----------------------------------------------------------------- */

void describe_module( char *filename )
{
    void * library = NULL;
    char ** globals_def = NULL;
    char ** locals_def = NULL;
    DLCONSTANT * constants_def = NULL;
    DLSYSFUNCS * functions_exports = NULL;
    char ** modules_dependency = NULL;
    DLVARFIXUP * globals_fixup = NULL;
    DLVARFIXUP * locals_fixup = NULL;
    HOOK * handler_hooks = NULL;
    void ( * RegisterFunctions )( void *( * )( const char * ), int ( * )( char *, char *, int, void * ) ) = NULL ;
    int misc = 0;

    char soname[1024];
    char * ptr;

#ifdef WIN32
#define DLLEXT      ".dll"
#define SIZEDLLEXT  4
#elif defined (TARGET_MAC)
#define DLLEXT      ".dylib"
#define SIZEDLLEXT  6
#else
#define DLLEXT      ".so"
#define SIZEDLLEXT  3
#endif

    snprintf( soname, 1024, "%s" DLLEXT, filename );

    for ( ptr = soname ; *ptr ; ptr++ )
    {
        if ( *ptr == PATH_CHAR_SEP )
            *ptr = PATH_CHAR_ISEP ;
        else
            *ptr = tolower( *ptr );
    }

    if ( strlen( soname ) > ( 4 + SIZEDLLEXT ) && strcmp( ptr - ( 4 + SIZEDLLEXT ), ".dll" DLLEXT ) == 0 ) strcpy( ptr - ( 4 + SIZEDLLEXT ), DLLEXT ); // .dll+.ext
    if ( strlen( soname ) > ( SIZEDLLEXT + SIZEDLLEXT ) && strcmp( ptr - ( SIZEDLLEXT + SIZEDLLEXT ), DLLEXT DLLEXT ) == 0 ) strcpy( ptr - ( SIZEDLLEXT + SIZEDLLEXT ), DLLEXT );

    filename = soname;

    library  = dlibopen( filename ) ;
    if ( !library )
    {
        printf( "ERROR: %s library not found\n", filename ) ;
        return;
    }

    printf( "Module name: %s\n\n", filename );

    constants_def = ( DLCONSTANT * ) _dlibaddr( library, "constants_def" ) ;
    if ( constants_def )
    {
        printf( "Constants:\n\n" ); fflush( stdout );
        while ( constants_def->name )
        {
            describe_type( constants_def->type ); fflush( stdout );
            printf( " %s = %d\n", constants_def->name, constants_def->code ); fflush( stdout );
            constants_def++ ;
        }
        printf( "\n\n" );
    }
    globals_def = ( char ** ) _dlibaddr( library, "globals_def" ) ;
    if ( globals_def && *globals_def )
    {
        printf( "Globals:\n\n%s\n\n", *globals_def );
    }

    locals_def  = ( char ** ) _dlibaddr( library, "locals_def" ) ;
    if ( locals_def && *locals_def )
    {
        printf( "Locals:\n\n%s\n\n", *locals_def );
    }

    functions_exports = ( DLSYSFUNCS * ) _dlibaddr( library, "functions_exports" ) ;
    if ( functions_exports )
    {
        printf( "Functions:\n\n" );
        while ( functions_exports->name )
        {
            describe_func( functions_exports->name, functions_exports->paramtypes, functions_exports->type, NULL );
            functions_exports++;
        }
        printf( "\n\n" );
    }

    modules_dependency = ( char ** ) _dlibaddr( library, "modules_dependency" ) ;
    if ( modules_dependency )
    {
        printf( "Module Dependency:\n\n" );
        while ( *modules_dependency )
        {
            printf( "%s\n", *modules_dependency );
            modules_dependency++;
        }
        printf( "\n\n" );
    }

    globals_fixup     = ( DLVARFIXUP * ) _dlibaddr( library, "globals_fixup" ) ;
    if ( globals_fixup )
    {
        printf( "Globals vars dependency:\n\n" );
        while ( globals_fixup->var )
        {
            printf( "%s\n", globals_fixup->var );
            globals_fixup++;
        }
        printf( "\n\n" );
    }

    locals_fixup      = ( DLVARFIXUP * ) _dlibaddr( library, "locals_fixup" ) ;
    if ( locals_fixup )
    {
        printf( "Locals vars dependency:\n\n" );
        while ( locals_fixup->var )
        {
            printf( "%s\n", locals_fixup->var );
            locals_fixup++;
        }
        printf( "\n\n" );
    }

#define CallbackHeaders() if (!misc) { misc = 1; printf ("Internals callbacks:\n\n"); }

    if ( _dlibaddr( library, "module_initialize" ) )
    {
        CallbackHeaders(); printf( "module_initialize\n" );
    }
    if ( _dlibaddr( library, "module_finalize" ) )
    {
        CallbackHeaders(); printf( "module_finalize\n" );
    }
    if ( _dlibaddr( library, "instance_create_hook" ) )
    {
        CallbackHeaders(); printf( "instance_create_hook\n" );
    }
    if ( _dlibaddr( library, "instance_destroy_hook" ) )
    {
        CallbackHeaders(); printf( "instance_destroy_hook\n" );
    }
    if ( _dlibaddr( library, "instance_pre_execute_hook" ) )
    {
        CallbackHeaders(); printf( "instance_pre_execute_hook\n" );
    }
    if ( _dlibaddr( library, "instance_pos_execute_hook" ) )
    {
        CallbackHeaders(); printf( "instance_pos_execute_hook\n" );
    }
    if ( _dlibaddr( library, "process_exec_hook" ) )
    {
        CallbackHeaders(); printf( "process_exec_hook\n" );
    }

    if ( misc ) printf( "\n\n" );

    handler_hooks = ( HOOK * ) _dlibaddr( library, "handler_hooks" ) ;
    if ( handler_hooks )
    {
        int i = 0;
        printf( "Hooks at priorities: " );
        while ( handler_hooks->hook )
        {
            printf( "%d", handler_hooks->prio );
            handler_hooks++;
            if ( handler_hooks->hook ) printf( ", " );
            i++;
        }
        printf( "\n\n" );
    }

    RegisterFunctions = _dlibaddr( library, "RegisterFunctions" ) ;
    if ( RegisterFunctions )
    {
        printf( "Fenix support:\n\n" );
        ( *RegisterFunctions )( fnc_import, describe_func ) ;
    }
}

/* ----------------------------------------------------------------- */

void help( char *argv[] )
{
    printf( "Usage: %s [options] modulename \n"
            "\n"
            "    -e     On fenix, only report exported functions\n"
            "    -i[-]  Enable/disable interactive mode (default: on)\n"
            "    -h     This help\n"
            "\n", argv[0] ) ;

    if ( interactive )
    {
        printf( "\nPress any key to continue...\n" );
        fflush( stdout );
        char a; scanf( "%c", &a ) ;
    }
}

/* ----------------------------------------------------------------- */

int main( int argc, char *argv[] )
{
    int i = 1;
    char * modname = NULL;

    printf( "Module Describe v1.1 (Build: %s %s)\n"
            "Copyright (C) 2008 SplinterGU\n"
            "This utility comes with ABSOLUTELY NO WARRANTY.\n"
            "%s -h for details\n\n", __DATE__, __TIME__, argv[0] ) ;

    if ( argc <= 1 )
    {
        help( argv ) ;
        exit( 0 ) ;
    }

    while ( i < argc )
    {
        if ( argv[i][0] == '-' )
        {
            switch ( argv[i][1] )
            {
                case 'e':
                    onlyfenixexport = 1 ;
                    break;

                case 'i':
                    switch ( argv[i][2] )
                    {
                        case '-':
                            interactive = 0 ;
                            break;

                        default:
                            interactive = 1 ;
                            break;
                    }
                    break;

                default:
                case 'h':
                    help( argv ) ;
                    exit( 0 );
                    break;
            }
        }
        else
        {
            modname = argv[i];
        }
        i++;
    }

    if ( !modname )
    {
        help( argv ) ;
        exit( 0 ) ;
    }

    describe_module( modname );
    if ( interactive )
    {
        printf( "\nPress any key to continue...\n" );
        fflush( stdout );
        char a; scanf( "%c", &a ) ;
    }
    exit( 0 );
}

/* ----------------------------------------------------------------- */
