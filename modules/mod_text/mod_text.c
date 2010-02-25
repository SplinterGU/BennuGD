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

#include <stdlib.h>

#include "bgdrtm.h"

#include "bgddl.h"
#include "dlvaracc.h"

#include "xstrings.h"

#include "libgrbase.h"
#include "libfont.h"
#include "libtext.h"

/* --------------------------------------------------------------------------- */

DLCONSTANT __bgdexport( mod_text, constants_def)[] =
{
    { "ALL_TEXT"            , TYPE_INT  , 0                     },

    /* Alineacion de textos */
    { "ALIGN_TOP_LEFT"      , TYPE_INT  , ALIGN_TOP_LEFT        },
    { "ALIGN_TOP"           , TYPE_INT  , ALIGN_TOP             },
    { "ALIGN_TOP_RIGHT"     , TYPE_INT  , ALIGN_TOP_RIGHT       },
    { "ALIGN_CENTER_LEFT"   , TYPE_INT  , ALIGN_CENTER_LEFT     },
    { "ALIGN_CENTER"        , TYPE_INT  , ALIGN_CENTER          },
    { "ALIGN_CENTER_RIGHT"  , TYPE_INT  , ALIGN_CENTER_RIGHT    },
    { "ALIGN_BOTTOM_LEFT"   , TYPE_INT  , ALIGN_BOTTOM_LEFT     },
    { "ALIGN_BOTTOM"        , TYPE_INT  , ALIGN_BOTTOM          },
    { "ALIGN_BOTTOM_RIGHT"  , TYPE_INT  , ALIGN_BOTTOM_RIGHT    },

    { NULL                  , 0         , 0                     }
} ;

/* --------------------------------------------------------------------------- */
/* Funciones de visualización de textos */

static int modtext_write (INSTANCE * my, int * params)
{
    const char * text = string_get (params[4]) ;
    int r = text ? gr_text_new (params[0], params[1], params[2], params[3], text) : 0 ;
    string_discard (params[4]) ;
    return r ;
}

/* --------------------------------------------------------------------------- */

static int modtext_write_in_map (INSTANCE * my, int * params)
{
    const char * text = string_get (params[1]) ;
    GRAPH * gr ;
    if (!text) return 0 ;
    gr = gr_text_bitmap (params[0], text, params[2]) ;
    string_discard (params[1]) ;
    if (!gr) return 0 ;
    return gr->code ;
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : modtext_write_var
 *
 *  Creates a new text associated with a variable
 *
 *  FENIX LANG PARAMS:
 *      params[0] : fnt to use
 *      params[1] : X
 *      params[2] : Y
 *      params[3] : align
 *      params[4] : pointer
 *      params[5] : DCB_TYPE
 *
 *  FENIX RETURN VALUE:
 *     Text ID for the newly created text
 *
 */

static int modtext_write_var (INSTANCE * my, int * params)
{
    DCB_TYPEDEF * var = (DCB_TYPEDEF *)params[5] ;
    int t = 0 ;

    switch (var->BaseType[0])
    {
        case TYPE_FLOAT:
            t = TEXT_FLOAT ;
            break ;

        case TYPE_DWORD:
            t = TEXT_DWORD ;
            break ;

        case TYPE_WORD:
            t = TEXT_WORD ;
            break ;

        case TYPE_BYTE:
            t = TEXT_BYTE ;
            break ;

        case TYPE_STRING:
            t = TEXT_STRING ;
            break ;

        case TYPE_ARRAY:
            if (var->BaseType[1] == TYPE_CHAR)
            {
                t = TEXT_CHARARRAY;
                break;
            }
            break;

        case TYPE_SBYTE:
            t = TEXT_SBYTE;
            break;

        case TYPE_CHAR:
            t = TEXT_CHAR;
            break;

        case TYPE_SHORT:
            t = TEXT_SHORT;
            break;

        case TYPE_INT:
            t = TEXT_INT;
            break;

        case TYPE_POINTER:
            t = TEXT_POINTER;
            break;

        default:
            return -1;
            break ;
    }

    return gr_text_new_var (params[0], params[1], params[2], params[3], (void *)params[4], t) ;
}

/* --------------------------------------------------------------------------- */
/*
 *  Same as modtext_write_var, but param[5] not given and always set to VAR_STRING
 */

static int modtext_write_string (INSTANCE * my, int * params)
{
    return gr_text_new_var (params[0], params[1], params[2], params[3], (void *)params[4], TEXT_STRING) ;
}

/* --------------------------------------------------------------------------- */
/*
 *  Same as modtext_write_var, but param[5] not given and always set to VAR_INT
 */

static int modtext_write_int (INSTANCE * my, int * params)
{
    return gr_text_new_var (params[0], params[1], params[2], params[3], (void *)params[4], TEXT_INT) ;
}

/* --------------------------------------------------------------------------- */
/*
 *  Same as modtext_write_var, but param[5] not given and always set to VAR_FLOAT
 */

static int modtext_write_float (INSTANCE * my, int * params)
{
    return gr_text_new_var (params[0], params[1], params[2], params[3], (void *)params[4], TEXT_FLOAT) ;
}

/* --------------------------------------------------------------------------- */

static int modtext_move_text (INSTANCE * my, int * params)
{
    gr_text_move (params[0], params[1], params[2]) ;
    return 1;
}

/* --------------------------------------------------------------------------- */

static int modtext_delete_text (INSTANCE * my, int * params)
{
    gr_text_destroy (params[0]) ;
    return 1;
}

/* --------------------------------------------------------------------------- */

static int modtext_text_height (INSTANCE * my, int * params)
{
    const char * str = string_get (params[1]) ;
    int result = gr_text_height (params[0], ( const unsigned char * ) str) ;
    string_discard (params[1]) ;
    return result ;
}

/* --------------------------------------------------------------------------- */

static int modtext_text_width (INSTANCE * my, int * params)
{
    const char * str = string_get (params[1]) ;
    int result = gr_text_width (params[0], ( const unsigned char * ) str) ;
    string_discard (params[1]) ;
    return result ;
}

/* --------------------------------------------------------------------------- */

static int modtext_set_text_color (INSTANCE * my, int * params)
{
    gr_text_setcolor (params[0]) ;
    return 1 ;
}

/* --------------------------------------------------------------------------- */

static int modtext_get_text_color (INSTANCE * my, int * params)
{
    return (gr_text_getcolor()) ;
}

/* --------------------------------------------------------------------------- */

DLSYSFUNCS  __bgdexport( mod_text, functions_exports)[] = {

    /* Textos */
    { "WRITE"               , "IIIIS"   , TYPE_INT  , modtext_write             },
    { "WRITE_INT"           , "IIIIP"   , TYPE_INT  , modtext_write_int         },
    { "MOVE_TEXT"           , "III"     , TYPE_INT  , modtext_move_text         },
    { "DELETE_TEXT"         , "I"       , TYPE_INT  , modtext_delete_text       },
    { "WRITE_IN_MAP"        , "ISI"     , TYPE_INT  , modtext_write_in_map      },
    { "TEXT_WIDTH"          , "IS"      , TYPE_INT  , modtext_text_width        },
    { "TEXT_HEIGHT"         , "IS"      , TYPE_INT  , modtext_text_height       },
    { "GET_TEXT_COLOR"      , ""        , TYPE_INT  , modtext_get_text_color    },
    { "SET_TEXT_COLOR"      , "I"       , TYPE_INT  , modtext_set_text_color    },

    /* Extended Write */
    { "WRITE_VAR"           , "IIIIV++" , TYPE_INT  , modtext_write_var         },
    { "WRITE_FLOAT"         , "IIIIP"   , TYPE_INT  , modtext_write_float       },
    { "WRITE_STRING"        , "IIIIP"   , TYPE_INT  , modtext_write_string      },

    { 0                     , 0         , 0         , 0                     }
};

/* --------------------------------------------------------------------------- */

char * __bgdexport( mod_text, modules_dependency)[] =
{
    "libgrbase",
    "libblit",
    "libtext",
    "libfont",
    NULL
};

/* --------------------------------------------------------------------------- */
