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

#include <stdlib.h>
#include <string.h>

#define __LIB_RENDER
#include "librender.h"

/* --------------------------------------------------------------------------- */

static int sequencer = 0;

CONTAINER * sorted_object_list = NULL;

/* --------------------------------------------------------------------------- */

CONTAINER * search_container( int key )
{
    CONTAINER * ctr = NULL;

    if ( !sorted_object_list ) return NULL;

    for ( ctr = sorted_object_list; ctr && ctr->key > key; ctr = ctr->next );

    if ( ctr && ctr->key == key ) return ctr;

    return NULL;
}

/* --------------------------------------------------------------------------- */

CONTAINER * get_container( int key )
{
    CONTAINER * ctr = NULL, * prev_ctr = NULL, * new_ctr = NULL;

    if ( sorted_object_list )
    {
        for ( ctr = sorted_object_list; ctr && ctr->key > key; ctr = ctr->next )
        {
            prev_ctr = ctr;
        }

        if ( ctr && ctr->key == key ) return ctr;
    }

    new_ctr = ( CONTAINER * ) malloc( sizeof( CONTAINER ) );
    if ( !new_ctr ) return NULL;

    new_ctr->key = key;
    new_ctr->first_in_key = NULL;

    if ( ctr == sorted_object_list )
    {
        if ( !sorted_object_list )
        {
            /* Only happen "!sorted_object_list" when "ctr == sorted_object_list" (NULL) */
            sorted_object_list = new_ctr;
            new_ctr->next = NULL;
            new_ctr->prev = NULL;

            return new_ctr;
        }

        sorted_object_list = new_ctr;
    }

    if ( ctr )
    {
        new_ctr->next = ctr;
        new_ctr->prev = ctr->prev;
        if ( ctr->prev ) ctr->prev->next = new_ctr;
        ctr->prev = new_ctr;
    }
    else
    {
        // prev_ctr implicit exist
        new_ctr->next = NULL;
        new_ctr->prev = prev_ctr;
        prev_ctr->next = new_ctr;
    }

    return new_ctr;
}

/* --------------------------------------------------------------------------- */

void destroy_container( CONTAINER * ctr )
{
    if ( ctr->next ) ctr->next->prev = ctr->prev;
    if ( ctr->prev ) ctr->prev->next = ctr->next;
    if ( ctr == sorted_object_list ) sorted_object_list = ctr->next ;

    free( ctr );
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : gr_new_object
 *
 *  Register a visible object with a Z coordinate to be drawn
 *  by an user-defined function.
 *
 *  If your DLL does some screen operations, you should register
 *  a draw-type hook or an object. Any access to the screen
 *  surface outside those are invalid.
 *
 *  PARAMS :
 *      z               Z value of the object to be drawn
 *      info            Pointer to the object information function
 *                      (fills bounding box, returns 1 if changed since last frame)
 *      draw            Pointer to the object drawing function
 *      what            User-defined parameter that will be passed to "draw"
 *
 *  RETURN VALUE :
 *      An integer ID that uniquely identifies the object,
 *    or 0 if not enough memory
 */

int gr_new_object( int z, OBJ_INFO * info, OBJ_DRAW * draw, void * what )
{
    CONTAINER * ctr ;
    OBJECT * object = ( OBJECT * ) malloc( sizeof( OBJECT ) );

    if ( !object ) return 0;

    ctr = get_container( z );
    if ( !ctr )
    {
        free( object );
        return 0;
    }

    object->z = z;
    object->info = info;
    object->draw = draw;
    object->what = what;
    object->ready = 0;
    object->bbox.x = -2;
    object->bbox.y = -2;
    object->bbox.x2 = -2;
    object->bbox.y2 = -2;
    object->bbox_saved.x = -2;
    object->bbox_saved.y = -2;
    object->bbox_saved.x2 = -2;
    object->bbox_saved.y2 = -2;
    object->seq = 0;
    object->prev = NULL;

    if ( ctr->first_in_key )
    {
        ctr->first_in_key->prev = object;
        object->next = ctr->first_in_key;
    }
    else
    {
        object->next = NULL;
    }

    ctr->first_in_key = object;

    return ( int ) object;
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : gr_destroy_object
 *
 *  Unregister and remove a given object created by gr_new_object from memory
 *
 *  PARAMS :
 *      id              ID returned by gr_new_object
 *
 *  RETURN VALUE :
 *      None
 */

void gr_destroy_object( int id )
{
    CONTAINER * ctr ;
    OBJECT * object = ( OBJECT * ) id ;

    if ( !object ) return ;

    ctr = search_container( object->z );
    if ( !ctr ) return ;

    if ( object->next ) object->next->prev = object->prev;
    if ( object->prev ) object->prev->next = object->next;
    if ( object == ctr->first_in_key ) ctr->first_in_key = object->next;

    if ( !ctr->first_in_key ) destroy_container( ctr );

    /* Rects */
    if ( object->bbox.x != -2 )
        gr_mark_rect( object->bbox.x, object->bbox.y, object->bbox.x2 - object->bbox.x + 1, object->bbox.y2 - object->bbox.y + 1 );

    free( object );
}

/* --------------------------------------------------------------------------- */

void gr_update_objects_mark_rects( int restore, int dump )
{
    CONTAINER * ctr = NULL, * next_ctr, * fix_ctr;
    OBJECT * object, * next_object ;
    int ready;

    if ( !sorted_object_list ) return ;

    sequencer++;

    next_ctr = sorted_object_list;
    while (( ctr = next_ctr ) )
    {
        /* Get Next Container */
        next_ctr = ctr->next ;

        next_object = ctr->first_in_key;
        while (( object = next_object ) )
        {
            next_object = object->next ;

            if ( object->seq != sequencer )
            {
                /* Mark object as already proceed */
                object->seq = sequencer;

                /* Update key & get_info */
                ready = object->ready;
                object->changed = ( *object->info )( object->what, &object->bbox, &object->z, &object->ready );

                /* Move to correct container */
                if ( object->z != ctr->key )
                {
                    /* Remove from list */
                    if ( object->next ) object->next->prev = object->prev;
                    if ( object->prev ) object->prev->next = object->next;
                    if ( object == ctr->first_in_key ) ctr->first_in_key = object->next;

                    if ( !ctr->first_in_key ) destroy_container( ctr );

                    /* Get new or exist container */
                    fix_ctr = get_container( object->z );
                    if ( !fix_ctr ) continue; // Error

                    /* Put new object first */
                    if ( fix_ctr->first_in_key ) fix_ctr->first_in_key->prev = object;

                    object->prev = NULL;
                    object->next = fix_ctr->first_in_key;

                    fix_ctr->first_in_key = object;
                }

                if (
                    ( object->z != ctr->key ) ||
                    ( object->ready ^ ready ) ||
                    object->changed ||
                    object->bbox.x2 != object->bbox_saved.x2 || object->bbox.x != object->bbox_saved.x ||
                    object->bbox.y2 != object->bbox_saved.y2 || object->bbox.y != object->bbox_saved.y
                )
                {
                    if ( restore == 0 )
                        gr_mark_rect( object->bbox_saved.x, object->bbox_saved.y, object->bbox_saved.x2 - object->bbox_saved.x + 1, object->bbox_saved.y2 - object->bbox_saved.y + 1 );

                    object->bbox_saved = object->bbox ;
                }

                if (
                    !object->ready ||
                    object->bbox.x2 < 0 || object->bbox.x > scr_width - 1 ||
                    object->bbox.y2 < 0 || object->bbox.y > scr_height - 1 ) continue;

                if ( dump == 0 && object->changed && object->bbox.x != -2 )
                {
                    gr_mark_rect( object->bbox.x, object->bbox.y, object->bbox.x2 - object->bbox.x + 1, object->bbox.y2 - object->bbox.y + 1 );
                }
            }
        }
    }
}

/* --------------------------------------------------------------------------- */

void gr_draw_objects( REGION * updaterects, int count )
{
    CONTAINER * ctr = NULL;
    OBJECT * object;
    REGION * prect;
    int n;

    ctr = sorted_object_list;
    while ( ctr )
    {
        object = ctr->first_in_key;
        while ( object )
        {
            if ( object->ready )
            {
                prect = updaterects;
                for ( n = count; n--; prect++ )
                {
                    if (
                        object->bbox.x2 < prect->x || object->bbox.x > prect->x2 ||
                        object->bbox.y2 < prect->y || object->bbox.y > prect->y2
                    )
                        continue;
                    ( *object->draw )( object->what, prect ) ;
                }
            }
            object = object->next ;
        }
        ctr = ctr->next ;
    }
}

/* --------------------------------------------------------------------------- */

void gr_draw_objects_complete( void )
{
    CONTAINER * ctr = NULL;
    OBJECT * object;

    ctr = sorted_object_list;
    while ( ctr )
    {
        object = ctr->first_in_key;
        while ( object )
        {
            if ( object->ready )
                ( *object->draw )( object->what, NULL ) ;
            object = object->next ;
        }
        ctr = ctr->next ;
    }
}

/* --------------------------------------------------------------------------- */
