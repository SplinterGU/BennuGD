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

#ifndef __TEXT_H
#define __TEXT_H

/* --------------------------------------------------------------------------- */

#include "libgrbase.h"
#include "libfont.h"

/* --------------------------------------------------------------------------- */

#define MAX_TEXTS           512

// Tipos para gr_text_new_var

#define TEXT_TEXT           1
#define TEXT_STRING         2
#define TEXT_INT            3
#define TEXT_FLOAT          4
#define TEXT_WORD           5
#define TEXT_BYTE           6
#define TEXT_CHARARRAY      7
#define TEXT_SHORT          8
#define TEXT_DWORD          9
#define TEXT_SBYTE          10
#define TEXT_CHAR           11
#define TEXT_POINTER        12

/* Alineacion para textos */
#define ALIGN_TOP_LEFT      0
#define ALIGN_TOP           1
#define ALIGN_TOP_RIGHT     2
#define ALIGN_CENTER_LEFT   3
#define ALIGN_CENTER        4
#define ALIGN_CENTER_RIGHT  5
#define ALIGN_BOTTOM_LEFT   6
#define ALIGN_BOTTOM        7
#define ALIGN_BOTTOM_RIGHT  8

/* --------------------------------------------------------------------------- */

extern int fntcolor ;          /* Color for drawing bitmap text    */

/* --------------------------------------------------------------------------- */

extern void gr_text_setcolor( int c ) ;
extern int gr_text_getcolor() ;
extern int gr_text_new( int fontid, int x, int y, int centered, const char * text ) ;
extern int gr_text_new_var( int fontid, int x, int y, int centered, const void * var, int type ) ;
extern void gr_text_move( int textid, int x, int y ) ;
extern void gr_text_destroy( int textid ) ;
extern int gr_text_margintop( int fontid, const unsigned char * text ) ;
extern int gr_text_width( int fontid, const unsigned char * text ) ;
extern int gr_text_widthn( int fontid, const unsigned char * text, int n ) ;
extern int gr_text_height( int fontid, const unsigned char * text ) ;
extern int gr_text_put( GRAPH * dest, REGION * region, int fontid, int x, int y, const unsigned char * text ) ;
extern GRAPH * gr_text_bitmap( int fontid, const char * text, int centered ) ;

/* --------------------------------------------------------------------------- */

#endif
