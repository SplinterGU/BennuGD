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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef TARGET_BEOS
#include <posix/assert.h>
#else
#include <assert.h>
#endif

#include "offsets.h"
#include "bgdrtm.h"
#include "sysprocs_p.h"
#include "instance.h"
#include "xstrings.h"

#undef STACK_SIZE
#define STACK_SIZE 16384

/* ---------------------------------------------------------------------- */

#define INSTANCE_MIN_PRIORITY    -2048
#define INSTANCE_MAX_PRIORITY     2048
#define PRIORITIES              (INSTANCE_MAX_PRIORITY - INSTANCE_MIN_PRIORITY + 1)

/* ---------------------------------------------------------------------- */
/* Módulo de gestión de instancias, con las funciones de incialización y  */
/* destrucción, duplicado, etc.                                           */
/* ---------------------------------------------------------------------- */

//#define HASH8(id)   ((((id)>>24)&0x000000ff)^(((id)>>16)&0x000000ff)^(((id)>>8)&0x000000ff)^((id)&0x000000ff))
#define HASH8(id)   ((id)&0x000000ff)

INSTANCE ** hashed_by_id = NULL;
INSTANCE ** hashed_by_instance = NULL;

/*
INSTANCE ** hashed_by_priority = NULL;
INSTANCE ** hashed_by_type = NULL;
*/
INSTANCE * first_instance = NULL ;
INSTANCE * last_instance  = NULL ;

/* Priority lists */

static INSTANCE * first_by_priority     = NULL ;
static INSTANCE * iterator_by_priority  = NULL ;
static int        iterator_reset        = 1;

/* Dirty list: a list of all instances that need an update because
 * they changed its priority since the last execution
 */

INSTANCE * dirty_list = NULL;

static int instance_maxid =  FIRST_INSTANCE_ID ;

/* ---------------------------------------------------------------------- */

void instance_add_to_list_by_id( INSTANCE * r )
{
    int hash = HASH8( LOCDWORD( r, PROCESS_ID ) );

    if ( !hashed_by_id ) hashed_by_id = calloc( 256, sizeof( INSTANCE * ) );

    r->prev_by_id = NULL ;
    r->next_by_id = hashed_by_id[hash];
    if ( r->next_by_id ) r->next_by_id->prev_by_id = r ;
    hashed_by_id[hash] = r;
}

/* ---------------------------------------------------------------------- */

void instance_remove_from_list_by_id( INSTANCE * r )
{
    int hash = HASH8( LOCDWORD( r, PROCESS_ID ) );

    if ( r->prev_by_id ) r->prev_by_id->next_by_id = r->next_by_id ;
    if ( r->next_by_id ) r->next_by_id->prev_by_id = r->prev_by_id ;

    if ( hashed_by_id[hash] == r ) hashed_by_id[hash] = r->next_by_id ;
}

/* ---------------------------------------------------------------------- */

void instance_add_to_list_by_instance( INSTANCE * r )
{
    int hash = HASH8((( uint32_t )r ) >> 2 );

    if ( !hashed_by_instance ) hashed_by_instance = calloc( 256, sizeof( INSTANCE * ) );

    r->prev_by_instance = NULL ;
    r->next_by_instance = hashed_by_instance[hash];
    if ( r->next_by_instance ) r->next_by_instance->prev_by_instance = r ;
    hashed_by_instance[hash] = r;
}

/* ---------------------------------------------------------------------- */

void instance_remove_from_list_by_instance( INSTANCE * r )
{
    int hash = HASH8((( uint32_t )r ) >> 2 );

    if ( r->prev_by_instance ) r->prev_by_instance->next_by_instance = r->next_by_instance ;
    if ( r->next_by_instance ) r->next_by_instance->prev_by_instance = r->prev_by_instance ;

    if ( hashed_by_instance[hash] == r ) hashed_by_instance[hash] = r->next_by_instance ;
}

/* ---------------------------------------------------------------------- */

/*
 *  FUNCTION : instance_get
 *
 *  Returns a instance, given its ID. This is actually
 *  slow, it should use a better search method.
 *
 *  PARAMS :
 *      id              Integer ID of the instance
 *
 *  RETURN VALUE :
 *      Pointer to the found instance or NULL if not found
 */

INSTANCE * instance_get( int id )
{
    INSTANCE * i ;

    if ( !hashed_by_id || !id ) return NULL;
    i = hashed_by_id[HASH8( id )];
    while ( i )
    {
        if ( LOCDWORD( i, PROCESS_ID ) == ( uint32_t ) id ) return i ;
        i = i->next_by_id ;
    }

    return NULL ;
}

/* ---------------------------------------------------------------------- */

/*
 *  FUNCTION : instance_getid
 *
 *  Allocate and return a free instance identifier code.
 *  It must be an even number. It should reuse already freed
 *  identifiers, but there is little point currently because
 *  an array is not used to access them.
 *
 *  PARAMS :
 *      None
 *
 *  RETURN VALUE :
 *      A new, unused instance identifier
 */

int instance_getid()
{
    return instance_maxid++ ;
}

/* ---------------------------------------------------------------------- */

/*
 *  FUNCTION : instance_duplicate
 *
 *  Create a copy of an instance, using the same local/private data
 *    - Updates the instance list, adding the new instance
 *    - Marks all local and private strings
 *    - Updates all parents local family variables
 *
 *  The new instance uses the same code pointer as the father
 *  (it begins execution at the same line, instead of the first one)
 *
 *  PARAMS :
 *      father          Pointer to the original instance
 *
 *  RETURN VALUE :
 *      Pointer to the new instance
 */

INSTANCE * instance_duplicate( INSTANCE * father )
{
    INSTANCE * r, * brother ;
    int n, pid ;

    r = ( INSTANCE * ) calloc( 1, sizeof( INSTANCE ) ) ;
    assert( r != 0 ) ;

    r->pridata          = ( int * ) malloc( father->private_size + 4 ) ;
    r->pubdata          = ( int * ) malloc( father->public_size + 4 ) ;
    r->locdata          = ( int * ) malloc( local_size + 4 ) ;
    r->code             = father->code ;
    r->codeptr          = father->code ; // Chequear si no es necesario duplicar el stack
    r->exitcode         = father->exitcode ;
    r->errorcode        = father->errorcode ;
    r->proc             = father->proc ;
    r->call_level       = father->call_level ;

    r->switchval        = 0;
    r->switchval_string = 0;
    r->cased            = 0;

    r->breakpoint       = 0 ;

    r->private_size     = father->private_size ;
    r->public_size      = father->public_size ;
    r->first_run        = 1 ;

    if ( father->private_size > 0 ) memcpy( r->pridata, father->pridata, r->private_size ) ;
    if ( father->public_size > 0 ) memcpy( r->pubdata, father->pubdata, r->public_size ) ;
    if ( local_size > 0 ) memcpy( r->locdata, father->locdata, local_size ) ;

    /* Crea el proceso clónico como si lo hubiera llamado el padre */

    /* Inicializa datos de jerarquia */

    pid = instance_getid() ;

    LOCDWORD( r, PROCESS_ID )   = pid ;
    LOCDWORD( r, PROCESS_TYPE ) = LOCDWORD( father, PROCESS_TYPE ) ;
    LOCDWORD( r, SON )          = 0 ;
    LOCDWORD( r, SMALLBRO )     = 0 ;

    LOCDWORD( r, FATHER )       = LOCDWORD( father, PROCESS_ID ) ;
    brother = instance_get( LOCDWORD( father, SON ) ) ;
    if ( brother )
    {
        LOCDWORD( r, BIGBRO )         = LOCDWORD( brother, PROCESS_ID ) ;
        LOCDWORD( brother, SMALLBRO ) = pid ;
    }
    else
    {
        LOCDWORD( r, BIGBRO )         = 0 ;
    }
    LOCDWORD( father, SON )      = pid ;

    /* Actualiza las cuentas de uso de las cadenas */

    for ( n = 0; n < r->proc->string_count; n++ ) string_use( PRIDWORD( r, r->proc->strings[n] ) ) ;  /* Strings privadas */
    for ( n = 0; n < r->proc->pubstring_count; n++ ) string_use( PUBDWORD( r, r->proc->pubstrings[n] ) ) ; /* Strings publicas */
    for ( n = 0; n < local_strings; n++ ) string_use( LOCDWORD( r, localstr[n] ) ) ; /* Strings locales */

    /* Añade la instancia al final de la lista */

    r->next    = 0 ;
    r->prev    = last_instance ;
    if ( r->prev ) r->prev->next = r ;

    r->next_by_priority = NULL;
    r->prev_by_priority = NULL;

    last_instance = r ;
    if ( !first_instance )
    {
        first_instance      = r ;
        LOCDWORD( r, FATHER ) = 0 ;
    }

    instance_add_to_list_by_id( r );
    instance_add_to_list_by_instance( r );

    /* The called_by pointer should be set only when the caller
     * is waiting for this process to return */

    r->called_by = NULL;

    r->stack = malloc( STACK_SIZE );
//    memmove(r->stack, father->stack, (int)father->stack_ptr - (int)father->stack);
    r->stack_ptr = &r->stack[1];
    r->stack[0] = STACK_SIZE;

    /* Initialize list pointers */

    r->next_dirty = dirty_list;
    dirty_list    = r;
    r->is_dirty   = 1;

    LOCDWORD( r, STATUS ) = STATUS_RUNNING;

    if ( instance_create_hook_count )
        for ( n = 0; n < instance_create_hook_count; n++ )
            instance_create_hook_list[n]( r );

    return r ;
}

/* ---------------------------------------------------------------------- */

/*
 *  FUNCTION : instance_new
 *
 *  Create a new instance, using the default local/private data
 *    - Updates the instance list, adding the new instance
 *    - Marks all local and private strings
 *    - Updates all parents local family variables
 *
 *  PARAMS :
 *      proc            Pointer to the procedure definition
 *      father          Pointer to the father instance (may be NULL)
 *
 *  RETURN VALUE :
 *      Pointer to the new instance
 */

INSTANCE * instance_new( PROCDEF * proc, INSTANCE * father )
{
    INSTANCE * r, * brother;
    int n, pid;

    r = ( INSTANCE * ) calloc( 1, sizeof( INSTANCE ) ) ;
    assert( r != 0 ) ;

    r->pridata          = ( int * ) malloc( proc->private_size + 4 ) ;
    r->pubdata          = ( int * ) malloc( proc->public_size + 4 ) ;
    r->locdata          = ( int * ) malloc( local_size + 4 ) ;
    r->code             = proc->code ;
    r->codeptr          = proc->code ;
    r->exitcode         = proc->exitcode ;
    r->errorcode        = proc->errorcode ;
    r->proc             = proc ;
    r->call_level       = 0 ;

    r->switchval        = 0;
    r->switchval_string = 0;
    r->cased            = 0;

    r->breakpoint       = 0 ;

    r->private_size     = proc->private_size ;
    r->public_size      = proc->public_size ;
    r->first_run        = 1 ;

    if ( proc->private_size > 0 ) memcpy( r->pridata, proc->pridata, proc->private_size ) ;
    if ( proc->public_size > 0 ) memcpy( r->pubdata, proc->pubdata, proc->public_size ) ;
    if ( local_size > 0 ) memcpy( r->locdata, localdata, local_size ) ;

    /* Inicializa datos de jerarquia */

    pid = instance_getid() ;

    LOCDWORD( r, PROCESS_ID )   = pid ;
    LOCDWORD( r, PROCESS_TYPE ) = proc->type ;
    LOCDWORD( r, SON )          = 0 ;
    LOCDWORD( r, SMALLBRO )     = 0 ;

    if ( father )
    {
        LOCDWORD( r, FATHER )     = LOCDWORD( father, PROCESS_ID ) ;
        brother = instance_get( LOCDWORD( father, SON ) ) ;
        if ( brother )
        {
            LOCDWORD( r, BIGBRO )         = LOCDWORD( brother, PROCESS_ID ) ;
            LOCDWORD( brother, SMALLBRO ) = pid ;
        }
        else
        {
            LOCDWORD( r, BIGBRO )         = 0 ;
        }
        LOCDWORD( father, SON )    = pid ;
    }
    else
    {
        LOCDWORD( r, FATHER )     = 0 ;
        LOCDWORD( r, BIGBRO )     = 0 ;
    }

    /* Cuenta los usos de las variables tipo cadena */

    for ( n = 0; n < proc->string_count; n++ ) string_use( PRIDWORD( r, proc->strings[n] ) ) ;  /* Strings privadas */
    for ( n = 0; n < proc->pubstring_count; n++ ) string_use( PUBDWORD( r, proc->pubstrings[n] ) ) ; /* Strings publicas */
    for ( n = 0; n < local_strings; n++ ) string_use( LOCDWORD( r, localstr[n] ) ) ; /* Strings locales */

    /* Añade la instancia al final de la lista */

    r->next    = 0 ;
    r->prev    = last_instance ;
    if ( r->prev ) r->prev->next = r ;

    r->next_by_priority = NULL;
    r->prev_by_priority = NULL;

    last_instance = r ;
    if ( !first_instance )
    {
        first_instance      = r ;
        LOCDWORD( r, FATHER ) = 0 ;
    }

    instance_add_to_list_by_id( r );
    instance_add_to_list_by_instance( r );

    /* The called_by pointer should be set only when the caller
     * is waiting for this process to return */

    r->called_by = NULL;

    r->stack = malloc( STACK_SIZE );
    r->stack_ptr = &r->stack[1];
    r->stack[0] = STACK_SIZE;

    /* Initialize list pointers */

    r->next_dirty = dirty_list;
    dirty_list    = r;
    r->is_dirty   = 1;

    LOCDWORD( r, STATUS ) = STATUS_RUNNING;

    if ( instance_create_hook_count )
        for ( n = 0; n < instance_create_hook_count; n++ )
            instance_create_hook_list[n]( r );

    return r ;
}

/* ---------------------------------------------------------------------- */

INSTANCE * instance_getfather( INSTANCE * i )
{
    return instance_get( LOCDWORD( i, FATHER ) ) ;
}

/* ---------------------------------------------------------------------- */

INSTANCE * instance_getson( INSTANCE * i )
{
    return instance_get( LOCDWORD( i, SON ) ) ;
}

/* ---------------------------------------------------------------------- */

INSTANCE * instance_getbigbro( INSTANCE * i )
{
    return instance_get( LOCDWORD( i, BIGBRO ) ) ;
}

/* ---------------------------------------------------------------------- */

INSTANCE * instance_getsmallbro( INSTANCE * i )
{
    return instance_get( LOCDWORD( i, SMALLBRO ) ) ;
}

/* ---------------------------------------------------------------------- */

/*
 *  FUNCTION : instance_destroy_all
 *
 *  Destroy all instances. Simply calls instance_destroy
 *  for any and every instance in existence.
 *
 *  PARAMS :
 *      except          Don't destroy this instance (used for LET_ME_ALONE)
 *
 *  RETURN VALUE :
 *      None
 */

void instance_destroy_all( INSTANCE * except )
{
    INSTANCE * i, * next ;

    i = first_instance ;
    while ( i )
    {
        next = i->next ;
        if ( i != except ) instance_destroy( i ) ;
        i = next ;
    }
}

/* ---------------------------------------------------------------------- */

/*
 *  FUNCTION : instance_destroy
 *
 *  Destroy an instance, effectively
 *    - Updates any instance list, removing the given instance
 *    - Discards all local and private strings
 *    - Updates all parents local family variables
 *    - Frees any memory involved
 *
 *  PARAMS :
 *      r           Pointer to the instance
 *
 *  RETURN VALUE :
 *      None
 */

void instance_destroy( INSTANCE * r )
{
    INSTANCE * father = NULL, * bigbro = NULL, * smallbro = NULL;
    int n ;

    LOCDWORD( r, STATUS ) = STATUS_RUNNING;

    if ( instance_destroy_hook_count )
        for ( n = 0; n < instance_destroy_hook_count; n++ )
            instance_destroy_hook_list[n]( r );

    /* Actualiza la cuenta de referencia de las variables tipo string */

    for ( n = 0 ; n < r->proc->string_count ; n++ ) string_discard( PRIDWORD( r, r->proc->strings[n] ) ) ; /* Strings privadas */
    for ( n = 0 ; n < r->proc->pubstring_count ; n++ ) string_discard( PUBDWORD( r, r->proc->pubstrings[n] ) ) ; /* Strings publicas */
    for ( n = 0 ; n < local_strings ; n++ ) string_discard( LOCDWORD( r, localstr[n] ) ) ; /* Strings locales */

    /* Actualiza árbol DIV */

    bigbro = instance_get( LOCDWORD( r, BIGBRO ) ) ; /* Tengo hermano mayor? */
    if ( bigbro ) LOCDWORD( bigbro, SMALLBRO ) = LOCDWORD( r, SMALLBRO ) ; /* El hermano menor de mi hermano mayor es mi hermano menor */

    smallbro = instance_get( LOCDWORD( r, SMALLBRO ) ) ; /* Tengo hermano menor? */
    if ( smallbro ) LOCDWORD( smallbro, BIGBRO ) = LOCDWORD( r, BIGBRO ) ; /* El hermano mayor de mi hermano menor es mi hermano mayor */

    father = instance_get( LOCDWORD( r, FATHER ) ) ; /* Tengo padre? */
    if ( father && instance_get( LOCDWORD( father, SON ) ) == r ) LOCDWORD( father, SON ) = LOCDWORD( r, BIGBRO ); /* Si tengo padre y soy el hijo menor, mi hermano mayor pasa a ser el menor hijo de mi padre */

    /* Quita la instancia de la lista */

    if ( r->prev ) r->prev->next  = r->next ;
    if ( r->next ) r->next->prev = r->prev ;

    if ( first_instance == r ) first_instance = r->next ;
    if ( last_instance == r ) last_instance = r->prev ;

    /* Remove the instance from the priority list */

    if ( first_by_priority == r ) first_by_priority = r->next_by_priority;
    if ( r->prev_by_priority ) r->prev_by_priority->next_by_priority = r->next_by_priority;
    if ( r->next_by_priority ) r->next_by_priority->prev_by_priority = r->prev_by_priority;

    instance_remove_from_list_by_id( r );
    instance_remove_from_list_by_instance( r );

    /* Update iterator_by_priority if necessary */

    if ( iterator_by_priority == r ) iterator_by_priority = r->next_by_priority;

    /* Remove the instance from the dirty list */

    if ( dirty_list == r )
    {
        dirty_list = r->next_dirty;
    }
    else if ( r->is_dirty )
    {
        INSTANCE * i = dirty_list;
        while ( i )
        {
            if ( i->next_dirty == r )
            {
                i->next_dirty = r->next_dirty;
                break;
            }
            i = i->next_dirty;
        }
    }

    if ( r->stack ) free( r->stack ) ;

    if ( r->locdata ) free( r->locdata ) ;
    if ( r->pubdata ) free( r->pubdata ) ;
    if ( r->pridata ) free( r->pridata ) ;
    free( r ) ;
}

/* ---------------------------------------------------------------------- */

/*
 *  FUNCTION : instance_exists
 *
 *  Given an instance pointer, returns TRUE if it is still valid.
 *
 *  PARAMS :
 *      i               Pointer to the instance
 *
 *  RETURN VALUE :
 *      1 if the instance pointer is in the global instance list, 0 otherwise
 */

int instance_exists( INSTANCE * r )
{
    INSTANCE * i ;

    if ( !hashed_by_instance || !r ) return 0;
    i = hashed_by_instance[HASH8((( uint32_t )r )>>2 )];
    while ( i )
    {
        if ( r == i ) return 1 ;
        i = i->next_by_instance ;
    }

    return 0 ;
}

/* ---------------------------------------------------------------------- */

/*
 *  FUNCTION : instance_next_by_priority
 *
 *  Gets the next instance pointer until no more instances are
 *  returned. Instances are returned sorted by priority.
 *
 *  PARAMS :
 *      None
 *
 *  RETURN VALUE :
 *      Pointer to the next priority on the list or NULL
 *      if there is no more instances. The next call in this
 *      case will return a pointer to the first instance
 *      (the one with the lower priority)
 */

INSTANCE * instance_next_by_priority()
{
    INSTANCE * i = NULL ;
    INSTANCE * j = NULL ;
    INSTANCE * l = NULL ;

    if ( !iterator_by_priority )
    {
        // NULL will be returned once and then the list will be reset

        if ( !iterator_reset )
        {
            iterator_reset = 1;
            return NULL;
        }
        iterator_reset = 0;

        // Add all dirty instances to its place at the list
        i = dirty_list;

        while ( i )
        {
            int prio = LOCINT32( i, PRIORITY );
            // Check the priority value

            if ( prio < INSTANCE_MIN_PRIORITY ) LOCINT32( i, PRIORITY ) = INSTANCE_MIN_PRIORITY;
            if ( prio > INSTANCE_MAX_PRIORITY ) LOCINT32( i, PRIORITY ) = INSTANCE_MAX_PRIORITY;

            // Remove the instance from the list

            if ( i->prev_by_priority ) i->prev_by_priority->next_by_priority = i->next_by_priority;
            if ( i->next_by_priority ) i->next_by_priority->prev_by_priority = i->prev_by_priority;

            if ( first_by_priority == i ) first_by_priority = i->next_by_priority;

            // Add the instance to the list. The easy case is when there is
            // already some instance with the same priority.

            i->prev_by_priority = NULL;
            i->next_by_priority = NULL;

            j = first_by_priority;

            if ( !j )
            {
                // There are no instances in the list
                first_by_priority   = i;
            }
            else
            {
                l = NULL;
                while ( j )
                {
                    if ( !j->is_dirty && prio >= LOCINT32( j, PRIORITY ) )
                    {
                        i->prev_by_priority = j->prev_by_priority ;
                        i->next_by_priority = j;
                        if ( j->prev_by_priority ) j->prev_by_priority->next_by_priority = i;
                        j->prev_by_priority = i;
                        if ( first_by_priority == j ) first_by_priority = i;
                        break;
                    }
                    l = j;
                    j = j->next_by_priority ;
                }

                if ( !j )
                {
                    l->next_by_priority = i;
                    i->prev_by_priority = l;
                }
            }
            i->is_dirty = 0;
            i = i->next_dirty;
        }

        // Reset the dirty list

        dirty_list = NULL;

        if ( first_by_priority )
            iterator_by_priority = first_by_priority->next_by_priority;
        else
            iterator_by_priority = NULL;

        return first_by_priority;
    }

    i = iterator_by_priority;

    if ( iterator_by_priority ) iterator_by_priority = iterator_by_priority->next_by_priority;

    return i;
}

/* ---------------------------------------------------------------------- */

/*
 *  FUNCTION : instance_dirty
 *
 *  Adds an instance to the dirty instances list. This is a list of all
 *  instances which priority changed since the last execution.
 *
 *  PARAMS :
 *      i               Pointer to the instance
 *
 *  RETURN VALUE :
 *      None
 */

void instance_dirty( INSTANCE * i )
{
    if ( !i->is_dirty )
    {
        i->next_dirty = dirty_list;
        dirty_list = i;
        i->is_dirty = 1;
    }
}

/* ---------------------------------------------------------------------- */
