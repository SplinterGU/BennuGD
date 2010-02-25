/*
 *  Copyright © 2006-2010 SplinterGU (Fenix/Bennugd)
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
 * FILE        : sysprocs.h
 * DESCRIPTION : Defines Internals functions SPECS and handlers
 **
 */

/*
 *  In interpreter: SYSMACRO returns a function pointer
 *  In compiler:    Already defined to return 0
 */

#ifndef SYSMACRO
#define SYSMACRO(a) a
#endif

#define MAX_SYSPROCS 3072

/* WARNING!!!!
   This table must be sorted by element Code !!!! Don't unsort it !!!
   2006/11/09    Splinter (jj_arg@yahoo.com)
*/

#ifdef SYSPROCS_ONLY_DECLARE
extern SYSPROC sysprocs[MAX_SYSPROCS] ;
#else
SYSPROC sysprocs[MAX_SYSPROCS] =
{
    /* Internal use */
    { 0x01, "#COPY#"    , "PV++I" , TYPE_POINTER    , 5, SYSMACRO(bgd_copy_struct)      },
    { 0x02, "#MEMCOPY#" , "PPI"   , TYPE_INT        , 3, SYSMACRO(bgd_internal_memcopy) },

    { 0   , 0           , ""      , TYPE_UNDEFINED  , 0, 0                              }
} ;
#endif
