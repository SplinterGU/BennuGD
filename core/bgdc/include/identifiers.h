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

#ifndef __IDENTIFIERS_H
#define __IDENTIFIERS_H

typedef struct _identifier
{
	const char * name ;
	int    code ;
	int	   line ;		/* First USE for the identifier */
	int    f ;			/* file where the id was found */
	struct _identifier * next ;
}
identifier ;

/* Gestor de identificadores */

extern void identifier_init () ;
extern void identifier_dump () ;
extern int  identifier_add (const char * string) ;
extern int  identifier_add_as (const char * string, int code) ;
extern int  identifier_search (const char * string) ;
extern int  identifier_search_or_add (const char * string) ;
extern const char * identifier_name (int code) ;
extern int identifier_line (int code) ;
extern int identifier_file (int code) ;

extern int  identifier_next_code () ;

extern identifier * identifier_first() ;
extern identifier * identifier_next (identifier * id) ;

#endif
