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
#include <string.h>
#include <stdlib.h>
#ifdef TARGET_BEOS
#include <posix/assert.h>
#else
#include <assert.h>
#endif

#include "bgdc.h"

/* ---------------------------------------------------------------------- */
/* Gestor de procesos y bloques de código. Este módulo contiene funciones */
/* de utilidad para crear procesos y bloques de código así como otras que */
/* se emplean durante y después del compilado.                            */
/* ---------------------------------------------------------------------- */

PROCDEF * mainproc = 0 ;
int procdef_count = 0 ;

int procdef_maxid = -1 ;
PROCDEF ** procs = 0 ;
int procs_allocated = 0 ;

int procdef_getid()
{
    return ++procdef_maxid ;
}

PROCDEF * procdef_new (int typeid, int id)
{
    PROCDEF * proc = (PROCDEF *) calloc (1, sizeof(PROCDEF)) ;
    int n ;

    if (!proc)
    {
        fprintf (stdout, "procdef_new: out of memory\n") ;
        exit (1) ;
    }

    proc->pridata = segment_new() ;
    proc->privars = varspace_new() ;

    /* (2006/11/19 23:15 GMT-03:00, Splinter - jj_arg@yahoo.com) */
    proc->pubdata = segment_new() ;
    proc->pubvars = varspace_new() ;
    /* (2006/11/19 23:15 GMT-03:00, Splinter - jj_arg@yahoo.com) */

    proc->params    = -1 ;
    proc->defined   = 0 ;
    proc->declared  = 0 ;
    proc->type      = TYPE_DWORD ;
    proc->flags     = 0 ;

    proc->sentence_count = 0 ;
    proc->sentences      = 0 ;

    if (typeid >= procs_allocated)
    {
        procs_allocated = typeid + 15 ;
        procs = (PROCDEF **) realloc (procs, sizeof(PROCDEF **) * procs_allocated) ;
        if (!procs)
        {
            fprintf (stdout, "procdef_new: out of memory\n") ;
            exit (1) ;
        }
    }
    proc->typeid     = typeid ;
    proc->identifier = id ;
    procs[typeid]    = proc ;

    for (n = 0 ; n < MAX_PARAMS ; n++)
        proc->paramtype[n] = TYPE_UNDEFINED ;

    proc->exitcode  = 0 ;
    proc->errorcode = 0 ;

    codeblock_init (&proc->code) ;
    procdef_count++ ;
    return proc ;
}

PROCDEF * procdef_search (int id)
{
    int n ;

    for (n = 0 ; n <= procdef_maxid; n++)
        if (procs[n]->identifier == id) return procs[n] ;

    return 0 ;
}

PROCDEF * procdef_search_by_codeblock (CODEBLOCK * p)
{
    int n ;

    for (n = 0 ; n <= procdef_maxid; n++)
        if (&procs[n]->code == p) return procs[n] ;

    return 0 ;
}

PROCDEF * procdef_get (int typeid)
{
    return procs_allocated > typeid ? procs[typeid] : 0 ;
}

void procdef_destroy (PROCDEF * proc)
{
    varspace_destroy (proc->privars) ;
    segment_destroy  (proc->pridata) ;

    /* (2006/11/20 01:09 GMT-03:00, Splinter - jj_arg@yahoo.com) */
    varspace_destroy (proc->pubvars);
    segment_destroy  (proc->pubdata) ;

    procs[proc->typeid] = 0 ;
    free (proc->code.data) ;
    free (proc->code.loops) ;
    free (proc->code.labels) ;
    free (proc) ;

    procdef_count-- ;
}


/* Realiza acciones posteriores al compilado sobre el código:
 * - Convierte saltos de código de etiqueta a offset
 * - Convierte identificador de procesos en CALL o TYPE a typeid */

void program_postprocess ()
{
    int n ;

    for (n = 0 ; n <= procdef_maxid ; n++)
        codeblock_postprocess (&procs[n]->code) ;
}
