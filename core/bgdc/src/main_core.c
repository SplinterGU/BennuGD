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
#include <string.h>

#include "bgdc.h"

/* ----------------------------------------------------------------------- */
/* Este módulo contiene las definiciones de constantes, globales y locales */
/* predefinidos, así como el código que los carga y define cada una de las */
/* funciones del sistema (es llamado antes de empezar a compilar).         */
/* ----------------------------------------------------------------------- */

static struct
{
    char * name ;
    int    type ;
    int    code ;
}
constants_def[] =
{
    { "STATUS_DEAD"     , TYPE_DWORD, STATUS_DEAD         },
    { "STATUS_KILLED"   , TYPE_DWORD, STATUS_KILLED       },
    { "STATUS_RUNNING"  , TYPE_DWORD, STATUS_RUNNING      },
    { "STATUS_SLEEPING" , TYPE_DWORD, STATUS_SLEEPING     },
    { "STATUS_FROZEN"   , TYPE_DWORD, STATUS_FROZEN       },
    { "STATUS_WAITING"  , TYPE_DWORD, STATUS_WAITING_MASK },

    { "NULL"            , TYPE_DWORD, 0                   },

    { "FALSE"           , TYPE_DWORD, 0                   },
    { "TRUE"            , TYPE_DWORD, !0                  },

    { "OS_WIN32"        , TYPE_DWORD, OS_WIN32            },
    { "OS_LINUX"        , TYPE_DWORD, OS_LINUX            },
    { "OS_BEOS"         , TYPE_DWORD, OS_BEOS             },
    { "OS_MACOS"        , TYPE_DWORD, OS_MACOS            },
    { "OS_GP32"         , TYPE_DWORD, OS_GP32             },
    { "OS_DC"           , TYPE_DWORD, OS_DC               },
    { "OS_BSD"          , TYPE_DWORD, OS_BSD              },
    { "OS_GP2X"         , TYPE_DWORD, OS_GP2X             },
    { "OS_GP2X_WIZ"     , TYPE_DWORD, OS_GP2X_WIZ         },

    { "MIN_INT"         , TYPE_INT  , -2147483647L - 1    },
    { "MAX_INT"         , TYPE_INT  , 2147483647L         },
    { "MIN_DWORD"       , TYPE_DWORD, 0                   },
    { "MAX_DWORD"       , TYPE_DWORD, 0xffffffff          },

    { "MIN_SHORT"       , TYPE_SHORT, -32768              },
    { "MAX_SHORT"       , TYPE_SHORT, 32767               },
    { "MIN_WORD"        , TYPE_WORD , 0                   },
    { "MAX_WORD"        , TYPE_WORD , 0xffff              },

    { "MIN_SBYTE"       , TYPE_SBYTE, -128                },
    { "MAX_SBYTE"       , TYPE_SBYTE, 127                 },
    { "MIN_BYTE"        , TYPE_BYTE , 0                   },
    { "MAX_BYTE"        , TYPE_BYTE , 0xff                },

    { "MIN_CHAR"        , TYPE_BYTE , 0                   },
    { "MAX_CHAR"        , TYPE_BYTE , 0xff                },

    { NULL              , 0         , 0                   }
} ;

static char * globals_def =
    "argc;\n"
    "string argv[32];\n"
    "os_id = -1;\n";

static char * locals_def =
    "id;\n"
    "STRUCT reserved\n"
    "process_type;\n"
    "frame_percent;\n"
    "status = STATUS_RUNNING;\n"
    "saved_status = STATUS_RUNNING;\n"
    "saved_priority;\n"
    "END\n"

    "father;\n"
    "son;\n"
    "smallbro;\n"
    "bigbro;\n"
    "priority;\n";

void core_init()
{
    int i = 0, code ;

    while ( constants_def[i].name )
    {
        code = identifier_search_or_add( constants_def[i].name ) ;
        constants_add( code, typedef_new( constants_def[i].type ), constants_def[i].code ) ;
        i++ ;
    }

    token_init( globals_def, -1 ) ;
    compile_varspace( &global, globaldata, 1, 1, 0, NULL, DEFAULT_ALIGNMENT, 0 ) ;

    token_init( locals_def, -1 ) ;
    compile_varspace( &local, localdata, 1, 1, 0, NULL, DEFAULT_ALIGNMENT, 0 ) ;
}
