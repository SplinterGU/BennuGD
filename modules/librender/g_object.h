/*
 *  Copyright © 2006-2009 SplinterGU (Fenix/Bennugd)
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

#ifndef __OBJECT_H
#define __OBJECT_H

/* --------------------------------------------------------------------------- */

typedef int ( OBJ_INFO )( void * what, REGION * clip, int * key, int * ready );
typedef void ( OBJ_DRAW )( void * what, REGION * clip );

typedef struct _object
{
    int z ;
    OBJ_INFO * info ;
    OBJ_DRAW * draw ;
    void * what ;
    int changed ;
    int ready ;         /* Ready to draw */
    REGION bbox ;
    REGION bbox_saved ;

    int seq;

    struct _object * prev ;
    struct _object * next ;
}
OBJECT ;

typedef struct _container
{
    int key ;
    OBJECT * first_in_key ;

    struct _container * prev ;
    struct _container * next ;
}
CONTAINER ;

/* --------------------------------------------------------------------------- */

//extern CONTAINER * sorted_object_list;

/* --------------------------------------------------------------------------- */

extern CONTAINER * search_container( int key ) ;
extern CONTAINER * get_container( int key ) ;
extern void destroy_container( CONTAINER * ctr ) ;
extern int gr_new_object( int z, OBJ_INFO * info, OBJ_DRAW * draw, void * what );
extern void gr_destroy_object( int id ) ;
extern void gr_update_objects_mark_rects( int restore, int dump ) ;
extern void gr_draw_objects( REGION * updaterects, int count ) ;
extern void gr_draw_objects_complete( void ) ;

/* --------------------------------------------------------------------------- */

#endif
