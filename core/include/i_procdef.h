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

#ifndef __I_PROCDEF_H
#define __I_PROCDEF_H

#include <i_procdef_st.h>

extern int        local_strings ;
extern int      * localstr ;

extern PROCDEF  * mainproc ;
extern PROCDEF  * procs ;
extern int        procdef_count ;

extern PROCDEF  * procdef_get (int n) ;
extern PROCDEF  * procdef_get_by_name (char * name) ;
extern SYSPROC  * sysproc_get (int code) ;
extern int        sysproc_add (char * name, char * paramtypes, int type, void * func) ;
extern void       sysproc_init() ;

#endif
