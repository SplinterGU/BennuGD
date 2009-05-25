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

#ifndef __I_PROCDEF_ST_H
#define __I_PROCDEF_ST_H

#include "instance_st.h"

/* Process definitions, more simple that in the compiler */

typedef struct _procdef
{
	int * pridata ;
	int * pubdata ;

	int * code ;

	int exitcode ;
	int errorcode ;

	int * strings ;
	int * pubstrings ;

	int private_size ;
	int public_size ;

	int code_size ;

	int string_count ;
	int pubstring_count ;

	int params ;
	int id ;
	int type ;
	int flags ;
	char * name ;

    int breakpoint;
}
PROCDEF ;

#define PROC_USES_FRAME 	0x01
#define PROC_USES_LOCALS	0x02
#define PROC_FUNCTION   	0x04
#define PROC_USES_PUBLICS   0x08

/* System functions */

typedef int SYSFUNC (INSTANCE *, int *) ;
typedef struct _sysproc
{
	int       code ;
	char    * name ;
	char    * paramtypes ;
	int       type ;
	int       params ;
	SYSFUNC * func ;
	int       id ;
}
SYSPROC ;

#endif
