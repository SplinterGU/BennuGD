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

#ifndef __LIBGRBASE_H
#define __LIBGRBASE_H

#include <stdlib.h>
#include <stdint.h>

#include "g_pal.h"
#include "g_bitmap.h"
#include "g_clear.h"
#include "g_grlib.h"
#include "g_region.h"
#include "g_blendop.h"
#include "g_conversion.h"

extern PIXEL_FORMAT * std_pixel_format8 ;
extern PIXEL_FORMAT * std_pixel_format16 ;
extern PIXEL_FORMAT * std_pixel_format32 ;

extern PIXEL_FORMAT * sys_pixel_format ;

extern GRAPH * background ;
extern GRAPH * scrbitmap ;

#endif
