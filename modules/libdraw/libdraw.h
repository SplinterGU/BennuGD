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

#ifndef __DRAW_H
#define __DRAW_H

#include "libgrbase.h"

/* --------------------------------------------------------------------------- */

extern uint32_t drawing_stipple;

/* --------------------------------------------------------------------------- */

void draw_vline( GRAPH * dest, REGION * clip, int x, int y, int h );
void draw_hline( GRAPH * dest, REGION * clip, int x, int y, int w );
void draw_box( GRAPH * dest, REGION * clip, int x, int y, int w, int h );
void draw_rectangle( GRAPH * dest, REGION * clip, int x, int y, int w, int h );
void draw_circle( GRAPH * dest, REGION * clip, int x, int y, int r );
void draw_fcircle( GRAPH * dest, REGION * clip, int x, int y, int r );
void draw_line( GRAPH * dest, REGION * clip, int x, int y, int w, int h );
void draw_bezier( GRAPH * dest, REGION * clip, int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, int level );

/* --------------------------------------------------------------------------- */

#endif
