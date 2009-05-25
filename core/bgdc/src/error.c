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

#include "bgdc.h"
#include "errors.h"
#include "files.h"

/* Errors indexed table */
struct _errindex errtable[10] ;

/*
 *  Generate & populate the error table
 */

void err_buildErrorTable()
{

    file * fp ;
    char   fname[64] ;
    char   line[1024] ;
    char * token ;
    int    code ;
    int    size ;
    char * msg = NULL ;
    struct _errmsg * err = NULL ;
    int len;

    /* open the desired localized error file, check for existence if not open EN default */
    strcpy( fname, "msg/" ) ;
    strcat( fname, langinfo ) ;
    strcat( fname, ".msg" ) ;

    if ( !file_exists( fname ) ) strcpy( fname, "msg/en.msg" ) ;

    fp = file_open( fname, "r" ) ;

    if ( fp )
    {
        while ( !file_eof( fp ) )
        {
            len = file_qgets( fp, line, 1024 );
            if ( file_eof( fp ) ) break;
            if ( !len ) continue;
            /* Check for multiline... */
            if ( line[0] == '_' )
            {
                /* must resize the string and concatenate */
                if ( err )
                {
                    size = strlen( err->msg ) + strlen( line ) ;
                    msg = ( char * )calloc( size, sizeof( char ) ) ;
                    strcpy( msg, err->msg ) ;
                    line[0] = '\n' ;
                    strcat( msg, line ) ;
                    free( err->msg ) ;
                    err->msg = strdup( msg ) ;
                }
            }
            else
            {
                /* Check for comment */
                token = strtok( line, "#" ) ;
                code = 0 ;
                while ( token != NULL )
                {
                    /* don't process comments */
                    if ( strlen( token ) == 0 ) break ;
                    if ( !code ) code = atoi( token ) ;
                    else msg = token ;
                    token = strtok( NULL, "#" ) ;
                }
                if ( code ) err = err_addError( code, msg ) ;
            }
        }
        file_close( fp ) ;
    }
}

struct _errmsg * err_addError( int code, const char * msg )
{

    int idx ;
    struct _errmsg * err ;
    struct _errmsg * cerr ;

    /* build error object */

    err = calloc( 1, sizeof( struct _errmsg ) ) ;
    if ( err )
    {
        err->code = code ;
        err->msg  = strdup( msg ) ;
        err->prev = NULL ;
        err->next = NULL ;

        /* determine index to use... */
        idx = code / 100 ;

        /* find insertion point */

        cerr = errtable[idx].errlist ;

        if ( cerr )
        {
            while ( cerr->next ) cerr = cerr->next ;
            cerr->next = err ;
            err->prev = cerr ;
        }
        else
        {
            errtable[idx].errlist = err ;
        }

        errtable[idx].count++ ;
    }

    return err ;
}

void err_delError( struct _errmsg * err )
{

    int idx ;

    if ( !err ) return ;

    idx = err->code / 100 ;

    if ( err->next )
    {
        err->next->prev = err->prev ;
    }

    if ( err->prev )
        err->prev->next = err->next ;
    else
        errtable[idx].errlist = err->next ;

    free( err->msg ) ;
    free( err ) ;

    errtable[idx].count-- ;

}

void err_destroyErrorTable( void )
{

    int i ;

    for ( i = 0;i < 10;i++ )
    {
        while ( errtable[i].count > 0 )
        {
            err_delError( errtable[i].errlist ) ;
        }
    }
}

const char * err_getErrorByCode( int code )
{

    int idx ;
    struct _errmsg * err ;

    idx = code / 100 ;

    err = errtable[idx].errlist ;

    while ( err && err->code != code ) err = err->next ;

    if ( !err ) return 0 ;

    return err->msg ;
}
