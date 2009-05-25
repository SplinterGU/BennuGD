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

#include "token.h"
#include "identifiers.h"
#include "compiler.h"

/* ---------------------------------------------------------------------- */
/* Gestor de identificadores                                              */
/* ---------------------------------------------------------------------- */

static identifier * identifier_hash[64] ;
static int          identifier_code = 1 ;
int                 identifier_count = 0 ;

int identifier_hash_value (const char * string)
{
	int t = 0 ;
	const char * ptr = string ;

/*	while (*ptr) t = (t << 1) | *ptr++ ; */
	while (*ptr) t = (t << 3) | ( (*ptr++) & 0x07 ) ; /* Mejor dispersion en el hashing */

	return (t & 63) ;
}

identifier * identifier_first()
{
	int n ;

	for (n = 0 ; n < 64 ; n++)
	{
		if (identifier_hash[n])
			return identifier_hash[n] ;
	}
	return 0 ;
}

identifier * identifier_next (identifier * id)
{
	int n ;

	if (id->next) return id->next ;

	n = identifier_hash_value (id->name) ;
	for (n++ ; n < 64 ; n++)
	{
		if (identifier_hash[n])
			return identifier_hash[n] ;
	}
	return 0 ;
}

void identifier_init ()
{
	int i ;

	for (i = 0 ; i < 64 ; i++) identifier_hash[i] = 0 ;

	identifier_count = 0 ;
	identifier_code = 1 ;
}

void identifier_dump ()
{
	int i, ii ;
	identifier * ptr ;

	printf ("\n---- %d identifiers ----\n\n", identifier_count) ;
	for (i = 0 ; i < 64 ; i++)
	{
		ptr = identifier_hash[i] ;
		ii = 0;
		while (ptr)
		{
			ii++;
			printf ("%4d: %-32s [%04d] [%3d]\n", ptr->code, ptr->name, i, ii) ;
			ptr = ptr->next ;
		}
	}
}

int identifier_add_as (const char * string, int code)
{
	int hash = identifier_hash_value(string) ;
	identifier * w = (identifier *)calloc(1, sizeof(identifier)) ;

	if (!w)
	{
		fprintf (stdout, "identifier_add: out of memory\n") ;
		exit(1);
	}

	w->name = strdup(string) ;
	if (!w->name)
	{
		fprintf (stdout, "identifier_add: out of memory\n") ;
		exit(1);
	}
	w->line = line_count ; /* Save First appearance */
	w->f = current_file ;  /* Save File info */
	w->code = code ;
	w->next = identifier_hash[hash] ;
	identifier_hash[hash] = w ;
	identifier_count++ ;

	return 1 ;
}

int identifier_add (const char * string)
{
	int code = identifier_code++ ;
	if (!identifier_add_as (string, code)) return 0 ;
	return code ;
}

int identifier_search (const char * string)
{
	int hash = identifier_hash_value (string) ;
	identifier * ptr = identifier_hash[hash] ;

	while (ptr)
	{
		if (!ptr->name) return 0;
		if (ptr->name[0] == *string)
		{
			if (strcmp (string, ptr->name) == 0) break ;
		}
		ptr = ptr->next ;
	}
	return ptr ? ptr->code : 0 ;
}

/* Return line for the identifier */
int identifier_line (int code)
{
int i ;
	identifier * ptr ;

	for (i = 0 ; i < 64 ; i++)
	{
		ptr = identifier_hash[i] ;
		while (ptr)
		{
			if (ptr->code == code) {
				return ptr->line ;
			}
			ptr = ptr->next ;
		}
	}
	return 0 ;
}

/* Return file for the identifier */
int identifier_file (int code)
{
    int i ;
	identifier * ptr ;

	for (i = 0 ; i < 64 ; i++)
	{
		ptr = identifier_hash[i] ;
		while (ptr)
		{
			if (ptr->code == code) {
				return ptr->f ;
			}
			ptr = ptr->next ;
		}
	}
	return 0 ;
}

const char * identifier_name (int code)
{
	int i ;
	identifier * ptr ;

	for (i = 0 ; i < 64 ; i++)
	{
		ptr = identifier_hash[i] ;
		while (ptr)
		{
			if (ptr->code == code) {
				return ptr->name ;
			}
			ptr = ptr->next ;
		}
	}
	return 0 ;
}

int identifier_search_or_add (const char * string)
{
	int result ;

	result = identifier_search (string) ;
	return result ? result : identifier_add (string) ;
}

int identifier_next_code ()
{
	return identifier_code ;
}

int identifier_is_basic_type (int id)
{
	return (
		id == identifier_int ||
		id == identifier_short ||
		id == identifier_char ||
		id == identifier_dword ||
		id == identifier_word ||
		id == identifier_byte ||
		id == identifier_signed ||
		id == identifier_unsigned ||
		id == identifier_float ||
		id == identifier_string
		);
}

int identifier_is_type (int id)
{
	return identifier_is_basic_type(id) || segment_by_name(id) != NULL;
}

