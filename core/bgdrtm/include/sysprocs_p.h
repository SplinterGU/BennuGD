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
 * FILE        : sysprocs_p.h
 * DESCRIPTION : Defines prototipes for internal system functions
 */

#ifndef __SYSPROCS_P_H
#define __SYSPROCS_P_H

#include "sysprocs_st.h"

/* ---------------------------------------------------------------------- */

extern char * sysproc_name( int code ) ;

/* ---------------------------------------------------------------------- */

extern HOOK * handler_hook_list ;
extern int handler_hook_allocated ;
extern int handler_hook_count ;

extern INSTANCE_HOOK * instance_pre_execute_hook_list ;
extern int instance_pre_execute_hook_allocated ;
extern int instance_pre_execute_hook_count ;

extern INSTANCE_HOOK * instance_pos_execute_hook_list ;
extern int instance_pos_execute_hook_allocated ;
extern int instance_pos_execute_hook_count ;

extern INSTANCE_HOOK * instance_create_hook_list ;
extern int instance_create_hook_allocated ;
extern int instance_create_hook_count ;

extern INSTANCE_HOOK * instance_destroy_hook_list ;
extern int instance_destroy_hook_allocated ;
extern int instance_destroy_hook_count ;

extern INSTANCE_HOOK * process_exec_hook_list ;
extern int process_exec_hook_allocated ;
extern int process_exec_hook_count ;

extern FN_HOOK * module_initialize_list ;
extern int module_initialize_allocated ;
extern int module_initialize_count ;

extern FN_HOOK * module_finalize_list ;
extern int module_finalize_allocated ;
extern int module_finalize_count ;

/* ---------------------------------------------------------------------- */

#endif
