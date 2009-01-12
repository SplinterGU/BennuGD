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
#ifdef TARGET_BEOS
#include <posix/assert.h>
#else
#include <assert.h>
#endif

#include "bgdc.h"

/* ---------------------------------------------------------------------- */
/* Este módulo contiene funciones que muestran el equivalente en texto de */
/* una instrucción o mnemónico, o de un bloque de código completo         */
/* ---------------------------------------------------------------------- */

void codeblock_dump (CODEBLOCK * c)
{
	int i, n, showdir ;
	for (i = 0 ; i < c->current ; i += MN_PARAMS(c->data[i])+1)
	{
		showdir = 0 ;
		for (n = 0 ; n < c->label_count ; n++)
		{
			if (c->labels[n] == i)
			{
				if (!showdir) printf ("\n") ;
				printf ("Label %d:\n", n) ;
				showdir = 1 ;
			}
		}
		for (n = 1 ; n < c->loop_count ; n++)
		{
			if (c->loops[n*2] == i)
			{
				if (!showdir) printf ("\n") ;
				printf("Start %d:\n", n) ;
				showdir = 1 ;
			}
			if (c->loops[n*2+1] == i)
			{
				if (!showdir) printf ("\n") ;
				printf("End %d:\n", n) ;
				showdir = 1 ;
			}
		}
		if (showdir) printf ("\n%d:\n", i) ;

		printf ("\t") ;

		mnemonic_dump (c->data[i], c->data[i+1] );

	}
	printf ("%d:\n", i) ;
}

/* ---------------------------------------------------------------------- */
