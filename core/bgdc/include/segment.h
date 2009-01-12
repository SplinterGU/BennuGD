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

#ifndef __SEGMENT_H
#define __SEGMENT_H

#include <stdint.h>

#include "typedef.h"

/* Un segmento es una zona lineal de datos que puede crecer dinámicamente */

typedef struct _segment
{
	void    * bytes ;
	int	current ;
	int	reserved ;
	int	id ;
}
segment ;

extern segment * segment_new() ;
extern segment * segment_duplicate(segment * s) ;
extern segment * segment_get(int id) ;

/* Devuelven el offset del nuevo dato */
extern int       segment_add_as    (segment * s, int32_t value, BASETYPE t) ;
extern int       segment_add_dword (segment * s, int32_t value) ;
extern int       segment_add_word  (segment * s, int16_t value) ;
extern int       segment_add_byte  (segment * s, int8_t  value) ;
extern int       segment_add_from  (segment * s, segment * from) ;

extern void      segment_dump(segment *) ;
extern void      segment_destroy(segment *) ;
extern void      segment_copy(segment *, int base_offset, int total_length) ;
extern void      segment_alloc (segment * n, int count) ;

extern segment  * globaldata ;
extern segment  * localdata ;

/* Segmentos nombrados (para tipos definidos por el usuario */

extern segment * segment_by_name (int code) ;
extern void      segment_name    (segment * s, int code) ;

#endif
