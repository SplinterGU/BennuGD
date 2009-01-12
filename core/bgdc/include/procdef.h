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

#ifndef __PROCDEF_H
#define __PROCDEF_H

/* Procesos (un "PROCDEF" es, en realidad, simplemente su definición) */

#include "segment.h"
#include "varspace.h"
#include "codeblock.h"
#include "typedef.h"

#define MAX_PARAMS          256

#define PROC_USES_FRAME     0x01
#define PROC_USES_LOCALS    0x02
#define PROC_FUNCTION       0x04
#define PROC_USES_PUBLICS   0x08

typedef struct _sentence
{
    int file ;
    int line ;
    int col ;
    int offset ;
}
SENTENCE ;

typedef struct _procdef
{
    VARSPACE    * privars ;
    segment     * pridata ;

    /* (2006/11/19 23:15 GMT-03:00, Splinter - jj_arg@yahoo.com) */
    VARSPACE    * pubvars ;
    segment     * pubdata ;
    /* (2006/11/19 23:15 GMT-03:00, Splinter - jj_arg@yahoo.com) */

    int         typeid ;
    int         identifier ;
    int         params ;
    int         defined ;
    int         declared ;
    int         flags ;

    BASETYPE    paramtype[MAX_PARAMS] ;
    BASETYPE    type ;

    CODEBLOCK   code ;

    int         exitcode;
    int         errorcode;

    SENTENCE    * sentences ;
    int         sentence_count ;
}
PROCDEF ;

extern int procdef_count ;
extern int procdef_maxid ;

extern int       procdef_getid() ;
extern PROCDEF * procdef_new (int typeid, int identifier) ;
extern PROCDEF * procdef_get (int typeid) ;
extern PROCDEF * procdef_search (int identifier) ;
extern PROCDEF * procdef_search_by_codeblock (CODEBLOCK * p);
extern void      procdef_destroy(PROCDEF *) ;

/* Proceso "principal", el primero en definirse y ejecutarse */
extern PROCDEF * mainproc ;

#endif
