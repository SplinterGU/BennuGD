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

/*
 * FILE        : bgdc.h
 * DESCRIPTION : Base includes in compiler and some vars and const defines
 *
 * HISTORY:
 *
 */

#ifndef ENGLISH
#define ENGLISH
#endif

/*
 *  VERSION
 */

#define BGDC_VERSION     "BGDC " VERSION " (" __DATE__ " " __TIME__ ")"

/*
 *  HEADER FILES
 */

#include <messages.h>
#include <commons_defs.h>

#include "files.h"
#include "xctype.h"

/* ---------------------------------------------------------------------- */
/* Módulos generales de mantenimiento de datos                            */
/* ---------------------------------------------------------------------- */

#include "typedef.h"
#include "constants.h"
#include "identifiers.h"
#include "xstrings.h"

/* ---------------------------------------------------------------------- */
/* Trucos de portabilidad                                                 */
/* ---------------------------------------------------------------------- */

#include "arrange.h"

/* ---------------------------------------------------------------------- */
/* Compilador                                                             */
/* ---------------------------------------------------------------------- */

#include "segment.h"
#include "varspace.h"
#include "token.h"
#include "codeblock.h"
#include "procdef.h"
#include "compiler.h"

extern char * main_path ;

extern int autoinclude ;    /* Incluye ficheros en el DCB automáticamente */
extern int imports[] ;      /* Códigos de cadena con nombres de imports */
extern int nimports ;       /* Número de imports */

extern char langinfo[64] ;  /* language setting */

/* Funciones para guardar y cargar un fichero DCB */

#include "dcb.h"

extern void dcb_add_file (const char * filename) ;
extern int  dcb_save (const char * filename, int options, const char * stubname) ;
extern void dcb_settype (DCB_TYPEDEF * d, TYPEDEF * t) ;

/* Funciones del sistema (no definidas) */

typedef struct _sysproc
{
    int    code ;
    char * name ;
    char * paramtypes ;
    BASETYPE type ;
    int    params ;
    int    id ;

    /* For sysproc_list */
    struct _sysproc * next;
}
SYSPROC ;

extern int         sysproc_add    (char * name, char * paramtypes, int type, void * func);
extern SYSPROC *   sysproc_get    (int id) ;
extern SYSPROC * * sysproc_getall (int id) ;
extern char    *   sysproc_name   (int code) ;
extern void        sysproc_init() ;

/* Constantes */

extern void core_init() ;

#include "offsets.h"
#include "pslang.h"

