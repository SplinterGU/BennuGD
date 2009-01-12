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

#ifndef __BGDRTM_H
#define __BGDRTM_H

#include <stdint.h>

#include <commons_defs.h>

#include "arrange.h"
#include "i_procdef.h"
#include "files.h"
#include "dcb.h"
#include "xctype.h"
#include "instance.h"

/* --------------------------------------------------------------------------- */

extern void * globaldata ;
extern void * localdata ;
extern int    local_size ;
extern int  * localstr ;
extern int    local_strings ;

extern PROCDEF * procs ;
extern PROCDEF * mainproc ;
extern int procdef_count ;

extern char *appname;

extern int debug ;          /* 1 if running in debug mode                    */

/* Trace */
extern int debug_mode;

extern int exit_value;
extern int must_exit;

extern int force_debug;
extern int debug_next;

extern int trace_sentence;
extern INSTANCE * trace_instance;

/* --------------------------------------------------------------------------- */

extern int strncmpi( char * str1, char * str2, int sz );

extern int dcb_load( const char * filename ) ;
extern int dcb_load_from( file * fp, int offset ) ;

extern char * getid_name( unsigned int code );

extern void mnemonic_dump( int i, int param );

/* --------------------------------------------------------------------------- */

extern void bgdrtm_entry( int argc, char * argv[] );
extern void bgdrtm_exit( int n );

/* --------------------------------------------------------------------------- */

#endif
