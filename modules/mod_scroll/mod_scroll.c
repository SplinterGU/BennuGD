/*
 *  Copyright © 2006-2011 SplinterGU (Fenix/Bennugd)
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

#include "libvideo.h"

#include "libscroll.h"

/* --------------------------------------------------------------------------- */

static int mod_scroll_start( INSTANCE * my, int * params )
{
    scroll_start( params[0], params[1], params[2], params[3], params[4], params[5] ) ;
    return 1 ;
}

static int mod_scroll_stop( INSTANCE * my, int * params )
{
    scroll_stop( params[0] ) ;
    return 1 ;
}

static int mod_scroll_move( INSTANCE * my, int * params )
{
    scroll_update (params[0]) ;
    return 1 ;
}

/* ---------------------------------------------------------------------- */

DLSYSFUNCS __bgdexport( mod_scroll, functions_exports )[] =
{
    { "SCROLL_START"    , "IIIIII", TYPE_INT   , mod_scroll_start   },
    { "SCROLL_STOP"     , "I"     , TYPE_INT   , mod_scroll_stop    },
    { "SCROLL_MOVE"     , "I"     , TYPE_INT   , mod_scroll_move    },

    { "START_SCROLL"    , "IIIIII", TYPE_INT   , mod_scroll_start   },
    { "STOP_SCROLL"     , "I"     , TYPE_INT   , mod_scroll_stop    },
    { "MOVE_SCROLL"     , "I"     , TYPE_INT   , mod_scroll_move    },

    { 0                 , 0       , 0          , 0                  }
};

/* --------------------------------------------------------------------------- */

char * __bgdexport( mod_scroll, modules_dependency )[] =
{
    "libscroll",
    0
};

/* --------------------------------------------------------------------------- */
