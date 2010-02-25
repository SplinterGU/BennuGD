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

/* --------------------------------------------------------------------------- */

#include "bgddl.h"

#include "libgrbase.h"

/* --------------------------------------------------------------------------- */

PIXEL_FORMAT * std_pixel_format8  = NULL;
PIXEL_FORMAT * std_pixel_format16 = NULL;
PIXEL_FORMAT * std_pixel_format32 = NULL;

PIXEL_FORMAT * sys_pixel_format = NULL;

GRAPH * background = NULL ;
GRAPH * scrbitmap = NULL ;

/* --------------------------------------------------------------------------- */
/* Module initialization                                                       */

void __bgdexport( libgrbase, module_initialize )()
{
    std_pixel_format8  = bitmap_create_format( 8 ) ;
    std_pixel_format16 = bitmap_create_format( 16 ) ;
    std_pixel_format32 = bitmap_create_format( 32 ) ;

    grlib_init() ;
}

/* --------------------------------------------------------------------------- */
