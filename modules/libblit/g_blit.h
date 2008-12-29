/*
*  Copyright © 2006-2008 SplinterGU (Fenix/Bennugd)
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

#ifndef __BLIT_H
#define __BLIT_H

/* --------------------------------------------------------------------------- */

#include "libgrbase.h"

/* --------------------------------------------------------------------------- */
/* Flags para gr_blit                                                          */

#define B_HMIRROR       0x0001
#define B_VMIRROR       0x0002
#define B_TRANSLUCENT   0x0004
#define B_ALPHA         0x0008
#define B_ABLEND        0x0010
#define B_SBLEND        0x0020
#define B_NOCOLORKEY    0x0080

#define B_ALPHA_MASK    0xFF00
#define B_ALPHA_SHIFT   8

/* --------------------------------------------------------------------------- */

extern void gr_blit( GRAPH * dest, REGION * clip, int x, int y, int flags, GRAPH * gr ) ;
extern void gr_get_bbox( REGION * dest, REGION * clip, int x, int y, int flags, int angle, int scalex, int scaley, GRAPH * gr ) ;
extern void gr_rotated_blit( GRAPH * dest, REGION * clip, int x, int y, int flags, int angle, int scalex, int scaley, GRAPH * gr ) ;

/* --------------------------------------------------------------------------- */

#endif
