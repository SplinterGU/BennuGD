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

#ifndef __FILES_ST_H
#define __FILES_ST_H

#include <stdio.h>

/* Funciones de acceso a ficheros */
/* ------------------------------ */

/* Ahora mismo son casi wrappers de stdio.h, pero en el futuro
 * el tipo "file" puede ser una estructura y las funciones,
 * ofrecer soporte transparente para ficheros PAK, etc. */

#define F_XFILE  1
#define F_FILE   2
#define F_GZFILE 3

#include <zlib.h>

#define __MAX_PATH          260
#define	__MAX_DRIVE	        (3)
#define	__MAX_DIR	        256
#define	__MAX_FNAME	        256
#define	__MAX_EXT	        256

#ifdef WIN32
#define PATH_SEP "\\"
#define PATH_ISEP "/"
#define PATH_CHAR_SEP '\\'
#define PATH_CHAR_ISEP '/'
#define PATH_BACKSLASH
#else
#define PATH_SEP "/"
#define PATH_ISEP "\\"
#define PATH_CHAR_SEP '/'
#define PATH_CHAR_ISEP '\\'
#define PATH_SLASH
#endif


typedef struct
{
    int     type ;

    FILE *  fp ;
    gzFile  gz ;
    int     n ;
    int     error ;
	char	name[__MAX_PATH];
	long    pos ;
	int     eof ;
}
file ;

#endif
