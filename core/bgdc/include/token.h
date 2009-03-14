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

#ifndef __TOKEN_H
#define __TOKEN_H

#include <files.h>

#define MAX_SOURCES         4096
#define MAX_MACRO_PARAMS    256

/* Tokenizador */

/* Tipos de token */
#define IDENTIFIER 1
#define STRING     2
#define NUMBER     3
#define FLOAT      4
#define LABEL      5
#define NOTOKEN    6

extern struct _token
    {
        int type ;
        int code ;
        float value ;
        int file;
        int line;
    } token ;

extern void token_init( const char * source, int file ) ;
extern void token_next() ;
extern void token_back() ;
extern void token_dump() ;

extern int line_count ;
extern int current_file ;
extern int n_files ;
extern char files[MAX_SOURCES][__MAX_PATH] ;

/* Se exportan todos los token */
extern struct _token token ;
extern struct _token token_prev ;
extern struct _token token_saved ;

#endif
