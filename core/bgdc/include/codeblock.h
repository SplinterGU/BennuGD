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

#ifndef __CODEBLOCK_H
#define __CODEBLOCK_H

/* Bloques de código compilado. Contienen estructuras que sólo se emplean
 * durante el compilado: la función program_postprocess() independiza los
 * datos (CODEBLOCK.data[]) de dichas estructuras, que el intérprete
 * no necesita */

typedef struct _codeblock
{
	int * data ;
	int reserved ;
	int current ;
	int previous ;
	int previous2 ;

	int * loops ;
	int loop_count ;
	int loop_reserved ;
	int loop_active ;

	int * labels ;
	int label_count ;
	int label_reserved ;
	struct _labelsextra{
	    int name;
	    int file;
	    int line;
	} * labelsextra;

}
CODEBLOCK ;

typedef struct _codeblock_pos
{
	int current;
	int previous;
	int previous2;
}
CODEBLOCK_POS ;

extern void codeblock_init(CODEBLOCK * c) ;
extern void codeblock_add (CODEBLOCK * c, int code, int param) ;
extern void codeblock_add_block (CODEBLOCK * c, CODEBLOCK_POS from, CODEBLOCK_POS to);
extern void codeblock_loop_start (CODEBLOCK * c, int loop, int begin) ;
extern void codeblock_loop_end (CODEBLOCK * c, int loop, int end) ;
extern int  codeblock_loop_add (CODEBLOCK * c) ;
extern int  codeblock_label_add (CODEBLOCK * c, int identifier) ;
extern void codeblock_label_set (CODEBLOCK * c, int label, int offset) ;
extern int  codeblock_label_get (CODEBLOCK * c, int label);
extern int codeblock_label_get_id_by_name (CODEBLOCK * c, int name);
extern void codeblock_postprocess (CODEBLOCK * c) ;
extern void codeblock_dump (CODEBLOCK * c) ;
extern void mnemonic_dump (int i, int param) ;
extern void program_postprocess () ;

extern CODEBLOCK_POS codeblock_pos(CODEBLOCK * c);
extern void          codeblock_setpos(CODEBLOCK * c, CODEBLOCK_POS p);

#endif

