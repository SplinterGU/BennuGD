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

#ifndef __TYPEDEF_ST_H
#define __TYPEDEF_ST_H

/* Tipos de dato */

typedef enum {
	TYPE_UNDEFINED = 0,

	TYPE_INT	 = 1,
	TYPE_DWORD   = 2,
	TYPE_SHORT	 = 3,
	TYPE_WORD	 = 4,
	TYPE_SBYTE	 = 5,
	TYPE_BYTE	 = 6,

	TYPE_CHAR    = 8,
	TYPE_FLOAT	 = 9,

	TYPE_STRING  = 16,
	TYPE_ARRAY	 = 17,
	TYPE_STRUCT	 = 18,
	TYPE_POINTER = 19
}
BASETYPE ;

typedef struct _typechunk
{
	BASETYPE   type ;
	int	   count ;	/* Para type == TYPE_ARRAY */
}
TYPECHUNK ;

#define MAX_TYPECHUNKS 8

typedef struct _typedef
{
	TYPECHUNK	   chunk[MAX_TYPECHUNKS] ;
	int		   depth ;
	struct _varspace * varspace ;
}
TYPEDEF ;

#define typedef_is_numeric(t)   ((t).chunk[0].type < 16)
#define typedef_is_integer(t)   ((t).chunk[0].type < 8)
#define typedef_is_float(t)     ((t).chunk[0].type == TYPE_FLOAT)
#define typedef_is_string(t)    ((t).chunk[0].type == TYPE_STRING)
#define typedef_is_struct(t)    ((t).chunk[0].type == TYPE_STRUCT)
#define typedef_is_array(t)     ((t).chunk[0].type == TYPE_ARRAY)
#define typedef_is_pointer(t)   ((t).chunk[0].type == TYPE_POINTER)
#define typedef_count(t)        ((t).chunk[0].count)
#define typedef_base(t)         ((t).chunk[0].type)
#define typedef_members(t)      ((t).varspace)
#define typedef_is_unsigned(t)	((t).chunk[0].type <= 8 && (t).chunk[0].type > 0 && !((t).chunk[0].type & 1))

#endif
