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

#include "bgdrtm.h"
#include "dcb.h"

#include "sysprocs_p.h"
#include "pslang.h"
#include "instance.h"
#include "offsets.h"
#include "xstrings.h"

/* ---------------------------------------------------------------------- */
/* Interpreter's main module                                              */
/* ---------------------------------------------------------------------- */

int debug_mode = 0 ;

int exit_value = 0 ;
int must_exit = 0 ;

int force_debug = 0 ;
int debug_next = 0 ;

int trace_sentence  = -1;
INSTANCE * trace_instance = NULL;

/* --------------------------------------------------------------------------- */

static INSTANCE * last_instance_run = NULL;

/* ---------------------------------------------------------------------- */

#ifdef DEBUG
void stack_dump( INSTANCE * r )
{
    int * ptr = &r->stack[1] ;
    int i = 0;

    while ( ptr < r->stack_ptr )
    {
        printf( "%08X ", *ptr++ ) ;
        if ( i == 4 )
        {
            i = 0;
            printf( "\n" );
        }
        else
        {
            i++;
        }
    }
    if ( i ) printf( "\n" );
    return ;
}
#endif

/* ---------------------------------------------------------------------- */

int instance_go_all()
{
    INSTANCE * i = NULL;
    int i_count = 0 ;
    int n;
    int status;

    must_exit = 0 ;

    while ( first_instance )
    {
        if ( debug_mode )
        {
            /* Hook */
            if ( handler_hook_count )
                for ( n = 0; n < handler_hook_count; n++ )
                    handler_hook_list[n].hook();
            /* Hook */
            if ( must_exit ) break ;

        }
        else
        {
            if ( last_instance_run )
            {
                if ( instance_exists( last_instance_run ) )
                {
                    i = last_instance_run;
                }
                else
                {
                    last_instance_run = NULL;
                    i = instance_next_by_priority();
                }
            }
            else
            {
                i = instance_next_by_priority();
                i_count = 0 ;
            }

            while ( i )
            {
                status = LOCDWORD( i, STATUS );
                // Si la instancia esta KILLED o DEAD o salio de algun comando debug, entonces vuelvo a ejecutarla
                if ( status == STATUS_KILLED || status == STATUS_DEAD || last_instance_run )
                {
                    // Run instance
                }
                else if ( status == STATUS_RUNNING && LOCINT32( i, FRAME_PERCENT ) < 100 )
                {
                    // Run instance
                    /* Hook */
                    if ( process_exec_hook_count )
                        for ( n = 0; n < process_exec_hook_count; n++ )
                            process_exec_hook_list[n]( i );
                    /* Hook */
                }
                else
                {
                    i = instance_next_by_priority();
                    last_instance_run = NULL;
                    continue;
                }

                i_count++;

                last_instance_run = NULL;

                instance_go( i );

                if ( force_debug )
                {
                    debug_mode = 1;
                    last_instance_run  = trace_instance;
                    break;
                }

                if ( must_exit ) break;

                i = instance_next_by_priority();
            }

            if ( must_exit ) break ;

            /* Si ya se completo un frame: Dibujar, actualizar variables, etc. */

            if ( !i_count && !force_debug )
            {
                /* Honors the signal-changed status of the process and
                 * saves so it is used in this loop the next frame
                 */

                i = first_instance ;
                while ( i )
                {
                    status = LOCDWORD( i, STATUS );
                    if ( status == STATUS_RUNNING ) LOCINT32( i, FRAME_PERCENT ) -= 100 ;

                    LOCDWORD( i, SAVED_STATUS ) = status ;

                    if ( LOCINT32( i, SAVED_PRIORITY ) != LOCINT32( i, PRIORITY ) )
                    {
                        LOCINT32( i, SAVED_PRIORITY ) = LOCINT32( i, PRIORITY );
                        instance_dirty( i );
                    }

                    i = i->next ;
                }

                if ( !first_instance ) break ;

                /* Hook */
                if ( handler_hook_count )
                    for ( n = 0; n < handler_hook_count; n++ )
                        handler_hook_list[n].hook();
                /* Hook */

                continue ;
            }

            /* NOTA: los procesos pueden haberse autodestruido tras su ejecución */
        }
    }

    return exit_value;

}

/* ---------------------------------------------------------------------- */

int instance_go( INSTANCE * r )
{
    if ( !r ) return 0 ;

    register int * ptr = r->codeptr ;

    int n, return_value = LOCDWORD( r, PROCESS_ID ) ;
    SYSPROC * p = NULL ;
    INSTANCE * i = NULL ;
    static char buffer[16];
    char * str = NULL ;
    int status ;

    /* Pointer to the current process's code (it may be a called one) */

    int child_is_alive = 0;

    /* ------------------------------------------------------------------------------- */
    /* Restauro si salio por debug */

    if ( debug )
    {
        printf( "***** INSTANCE %s(%d) ENTRY StackBase=%p StackPTR=%p\n", r->proc->name, LOCDWORD( r, PROCESS_ID ), r->stack, r->stack_ptr ) ;
        fflush( stdout );
    }

    /* Hook */
    if ( instance_pre_execute_hook_count )
    {
        for ( n = 0; n < instance_pre_execute_hook_count; n++ )
            instance_pre_execute_hook_list[n]( r );
    }
    /* Hook */

    if (( r->proc->breakpoint || r->breakpoint ) && trace_instance != r )
    {
        debug_next = 1;
    }

    trace_sentence = -1;

    while ( !must_exit )
    {
        /* Si me killearon o estoy en waiting salgo */
        status = LOCDWORD( r, STATUS );
        if (( status & ~STATUS_WAITING_MASK ) == STATUS_KILLED || ( status & STATUS_WAITING_MASK ) )
        {
            r->codeptr = ptr;
            return_value = LOCDWORD( r, PROCESS_ID );
            goto break_all ;
        }

        if ( debug_next && trace_sentence != -1 )
        {
            force_debug = 1;
            debug_next = 0;
            r->codeptr = ptr ;
            return_value = LOCDWORD( r, PROCESS_ID );
            break;
        }

#ifdef DEBUG
        if ( debug )
        {
            stack_dump( r );
            printf( "%45s [%4u] ", " ", ( ptr - r->code ) ) ;
            mnemonic_dump( *ptr, ptr[1] ) ;
            printf( "\n" );
            fflush( stdout ) ;
        }
#endif

        switch ( *ptr )
        {
                /* Manipulación de la pila */

            case MN_DUP:
                *r->stack_ptr++ = r->stack_ptr[-1] ;
                ptr++ ;
                break ;

            case MN_PUSH:
//            case MN_PUSH | MN_FLOAT:
                *r->stack_ptr++ = ptr[1] ;
                ptr += 2 ;
                break ;

            case MN_POP:
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_INDEX:
            case MN_INDEX | MN_UNSIGNED:
            case MN_INDEX | MN_STRING:
            case MN_INDEX | MN_WORD:
            case MN_INDEX | MN_WORD | MN_UNSIGNED:
            case MN_INDEX | MN_BYTE:
            case MN_INDEX | MN_BYTE | MN_UNSIGNED:
            case MN_INDEX | MN_FLOAT: /* Splinter, se agrega float, no se por que no estaba */
                r->stack_ptr[-1] += ptr[1] ;
                ptr += 2 ;
                break ;

            case MN_ARRAY:
                r->stack_ptr[-2] += ( ptr[1] * r->stack_ptr[-1] ) ;
                r->stack_ptr-- ;
                ptr += 2 ;
                break ;

                /* Llamadas a procesos */

            case MN_CLONE:
                i = instance_duplicate( r ) ;
                i->codeptr = ptr + 2 ;
                ptr = r->code + ptr[1] ;
                continue ;

            case MN_CALL:
            case MN_PROC:
            {
                PROCDEF * proc = procdef_get( ptr[1] ) ;

                if ( !proc )
                {
                    fprintf( stderr, "ERROR: Runtime error in %s(%d) - Unknown process\n", r->proc->name, LOCDWORD( r, PROCESS_ID ) ) ;
                    exit( 0 );
                }

                /* Process uses FRAME or locals, must create an instance */
                i = instance_new( proc, r ) ;

                for ( n = 0; n < proc->params; n++ )
                    PRIDWORD( i, 4 * n ) = r->stack_ptr[-proc->params+n] ;

                r->stack_ptr -= proc->params ;

                /* Por default, me pongo en espera... */
                LOCDWORD( r, STATUS ) |= STATUS_WAITING_MASK;
                i->called_by   = r;

                // Ejecuto la funcion/processo...
                if ( *ptr == MN_CALL )
                {
                    r->stack[0] |= STACK_RETURN_VALUE;
                    r->stack_ptr++;
                    *r->stack_ptr = instance_go( i );
                }
                else
                {
                    r->stack[0] &= ~STACK_RETURN_VALUE;
                    instance_go( i );
                }

                child_is_alive = instance_exists( i );

                ptr += 2 ;

                /* If the process is a function in a frame, save the stack and leave */
                /* Si sigue corriendo la funcion/proceso que lance, es porque esta en un frame.
                   Si esta ejecutando codigo, es porque su STATUS es RUNNING */
                if ( child_is_alive &&
                        (
                            (( status = LOCDWORD( r, STATUS ) ) &  STATUS_WAITING_MASK ) ||
                            ( status & ~STATUS_WAITING_MASK ) == STATUS_FROZEN ||
                            ( status & ~STATUS_WAITING_MASK ) == STATUS_SLEEPING
                        )
                   )
                {
                    /* En este caso me pongo a dormir y retorno */
                    i->called_by   = r;

                    /* Guardo el puntero de instruccion */
                    /* Esta instancia no va a ejecutar otro codigo hasta que retorne el hijo */
                    r->codeptr = ptr ;

                    /* Si no fue un call, seteo un flag en la len para no retornar valor */
                    if ( ptr[-2] == MN_CALL )
                        r->stack[0] |= STACK_RETURN_VALUE;
                    else
                        r->stack[0] &= ~STACK_RETURN_VALUE;

                    if ( debug_next && trace_sentence != -1 )
                    {
                        force_debug = 1;
                        debug_next = 0;
                    }
                    return 0;
                }

                /* Me despierto */
                LOCDWORD( r, STATUS ) &= ~STATUS_WAITING_MASK;
                if ( child_is_alive ) i->called_by = NULL;

                break ;
            }

            case MN_SYSCALL:
                p = sysproc_get( ptr[1] ) ;
                if ( !p )
                {
                    fprintf( stderr, "ERROR: Runtime error in %s(%d) - Unknown system function\n", r->proc->name, LOCDWORD( r, PROCESS_ID ) ) ;
                    exit( 0 );
                }
                r->stack_ptr -= p->params ;
                *r->stack_ptr = ( *p->func )( r, r->stack_ptr ) ;
                r->stack_ptr++ ;
                ptr += 2 ;
                break ;

            case MN_SYSPROC:
                p = sysproc_get( ptr[1] ) ;
                if ( !p )
                {
                    fprintf( stderr, "ERROR: Runtime error in %s(%d) - Unknown system process\n", r->proc->name, LOCDWORD( r, PROCESS_ID ) ) ;
                    exit( 0 );
                }
                r->stack_ptr -= p->params ;
                ( *p->func )( r, r->stack_ptr ) ;
                ptr += 2 ;
                break ;

                /* Acceso a direcciones de variables */

            case MN_PRIVATE:
            case MN_PRIVATE | MN_UNSIGNED:
            case MN_PRIVATE | MN_WORD:
            case MN_PRIVATE | MN_BYTE:
            case MN_PRIVATE | MN_WORD | MN_UNSIGNED:
            case MN_PRIVATE | MN_BYTE | MN_UNSIGNED:
            case MN_PRIVATE | MN_STRING:
            case MN_PRIVATE | MN_FLOAT:
                *r->stack_ptr++ = ( uint32_t ) & PRIDWORD( r, ptr[1] );
                ptr += 2 ;
                break ;

            case MN_PUBLIC:
            case MN_PUBLIC | MN_UNSIGNED:
            case MN_PUBLIC | MN_WORD:
            case MN_PUBLIC | MN_BYTE:
            case MN_PUBLIC | MN_WORD | MN_UNSIGNED:
            case MN_PUBLIC | MN_BYTE | MN_UNSIGNED:
            case MN_PUBLIC | MN_STRING:
            case MN_PUBLIC | MN_FLOAT:
                *r->stack_ptr++ = ( uint32_t ) & PUBDWORD( r, ptr[1] ) ;
                ptr += 2 ;
                break ;

            case MN_LOCAL:
            case MN_LOCAL | MN_UNSIGNED:
            case MN_LOCAL | MN_WORD:
            case MN_LOCAL | MN_BYTE:
            case MN_LOCAL | MN_WORD | MN_UNSIGNED:
            case MN_LOCAL | MN_BYTE | MN_UNSIGNED:
            case MN_LOCAL | MN_STRING:
            case MN_LOCAL | MN_FLOAT:
                *r->stack_ptr++ = ( uint32_t ) & LOCDWORD( r, ptr[1] ) ;
                ptr += 2 ;
                break ;

            case MN_GLOBAL:
            case MN_GLOBAL | MN_UNSIGNED:
            case MN_GLOBAL | MN_WORD:
            case MN_GLOBAL | MN_BYTE:
            case MN_GLOBAL | MN_WORD | MN_UNSIGNED:
            case MN_GLOBAL | MN_BYTE | MN_UNSIGNED:
            case MN_GLOBAL | MN_STRING:
            case MN_GLOBAL | MN_FLOAT:
                *r->stack_ptr++ = ( uint32_t ) & GLODWORD( ptr[1] ) ;
                ptr += 2 ;
                break ;

            case MN_REMOTE:
            case MN_REMOTE | MN_UNSIGNED:
            case MN_REMOTE | MN_WORD:
            case MN_REMOTE | MN_BYTE:
            case MN_REMOTE | MN_WORD | MN_UNSIGNED:
            case MN_REMOTE | MN_BYTE | MN_UNSIGNED:
            case MN_REMOTE | MN_STRING:
            case MN_REMOTE | MN_FLOAT:
                i = instance_get( r->stack_ptr[-1] ) ;
                if ( !i )
                {
                    fprintf( stderr, "ERROR: Runtime error in %s(%d) - Process %d not active\n", r->proc->name, LOCDWORD( r, PROCESS_ID ), r->stack_ptr[-1] ) ;
                    exit( 0 );
                }
                else
                    r->stack_ptr[-1] = ( uint32_t ) & LOCDWORD( i, ptr[1] ) ;
                ptr += 2 ;
                break ;

            case MN_REMOTE_PUBLIC:
            case MN_REMOTE_PUBLIC | MN_UNSIGNED:
            case MN_REMOTE_PUBLIC | MN_WORD:
            case MN_REMOTE_PUBLIC | MN_BYTE:
            case MN_REMOTE_PUBLIC | MN_WORD | MN_UNSIGNED:
            case MN_REMOTE_PUBLIC | MN_BYTE | MN_UNSIGNED:
            case MN_REMOTE_PUBLIC | MN_STRING:
            case MN_REMOTE_PUBLIC | MN_FLOAT:
                i = instance_get( r->stack_ptr[-1] ) ;
                if ( !i )
                {
                    fprintf( stderr, "ERROR: Runtime error in %s(%d) - Process %d not active\n", r->proc->name, LOCDWORD( r, PROCESS_ID ), r->stack_ptr[-1] ) ;
                    exit( 0 );
                }
                else
                    r->stack_ptr[-1] = ( uint32_t ) & PUBDWORD( i, ptr[1] ) ;
                ptr += 2 ;
                break ;

                /* Acceso a variables tipo DWORD */

            case MN_GET_PRIV:
            case MN_GET_PRIV | MN_FLOAT:
            case MN_GET_PRIV | MN_UNSIGNED:
                *r->stack_ptr++ = PRIDWORD( r, ptr[1] ) ;
                ptr += 2 ;
                break ;

            case MN_GET_PUBLIC:
            case MN_GET_PUBLIC | MN_FLOAT:
            case MN_GET_PUBLIC | MN_UNSIGNED:
                *r->stack_ptr++ = PUBDWORD( r, ptr[1] ) ;
                ptr += 2 ;
                break ;

            case MN_GET_LOCAL:
            case MN_GET_LOCAL | MN_FLOAT:
            case MN_GET_LOCAL | MN_UNSIGNED:
                *r->stack_ptr++ = LOCDWORD( r, ptr[1] ) ;
                ptr += 2 ;
                break ;

            case MN_GET_GLOBAL:
            case MN_GET_GLOBAL | MN_FLOAT:
            case MN_GET_GLOBAL | MN_UNSIGNED:
                *r->stack_ptr++ = GLODWORD( ptr[1] ) ;
                ptr += 2 ;
                break ;

            case MN_GET_REMOTE:
            case MN_GET_REMOTE | MN_FLOAT:
            case MN_GET_REMOTE | MN_UNSIGNED:
                i = instance_get( r->stack_ptr[-1] ) ;
                if ( !i )
                {
                    fprintf( stderr, "ERROR: Runtime error in %s(%d) - Process %d not active\n", r->proc->name, LOCDWORD( r, PROCESS_ID ), r->stack_ptr[-1] ) ;
                    exit( 0 );
                }
                else
                    r->stack_ptr[-1] = LOCDWORD( i, ptr[1] ) ;
                ptr += 2 ;
                break ;

            case MN_GET_REMOTE_PUBLIC:
            case MN_GET_REMOTE_PUBLIC | MN_FLOAT:
            case MN_GET_REMOTE_PUBLIC | MN_UNSIGNED:
                i = instance_get( r->stack_ptr[-1] ) ;
                if ( !i )
                {
                    fprintf( stderr, "ERROR: Runtime error in %s(%d) - Process %d not active\n", r->proc->name, LOCDWORD( r, PROCESS_ID ), r->stack_ptr[-1] ) ;
                    exit( 0 );
                }
                else
                    r->stack_ptr[-1] = PUBDWORD( i, ptr[1] ) ;
                ptr += 2 ;
                break ;

            case MN_PTR:
            case MN_PTR | MN_UNSIGNED:
            case MN_PTR | MN_FLOAT:
                r->stack_ptr[-1] = *( int32_t * )r->stack_ptr[-1] ;
                ptr++ ;
                break ;

                /* Acceso a variables tipo STRING */

            case MN_PUSH | MN_STRING:
                *r->stack_ptr++ = ptr[1];
                string_use( r->stack_ptr[-1] );
                ptr += 2 ;
                break ;

            case MN_GET_PRIV | MN_STRING:
                *r->stack_ptr++ = PRIDWORD( r, ptr[1] ) ;
                string_use( r->stack_ptr[-1] );
                ptr += 2 ;
                break ;

            case MN_GET_PUBLIC | MN_STRING:
                *r->stack_ptr++ = PUBDWORD( r, ptr[1] ) ;
                string_use( r->stack_ptr[-1] );
                ptr += 2 ;
                break ;

            case MN_GET_LOCAL | MN_STRING:
                *r->stack_ptr++ = LOCDWORD( r, ptr[1] ) ;
                string_use( r->stack_ptr[-1] );
                ptr += 2 ;
                break ;

            case MN_GET_GLOBAL | MN_STRING:
                *r->stack_ptr++ = GLODWORD( ptr[1] ) ;
                string_use( r->stack_ptr[-1] );
                ptr += 2 ;
                break ;

            case MN_GET_REMOTE | MN_STRING:
                i = instance_get( r->stack_ptr[-1] ) ;
                if ( !i )
                {
                    fprintf( stderr, "ERROR: Runtime error in %s(%d) - Process %d not active\n", r->proc->name, LOCDWORD( r, PROCESS_ID ), r->stack_ptr[-1] ) ;
                    exit( 0 );
                }
                else
                    r->stack_ptr[-1] = LOCDWORD( i, ptr[1] ) ;
                string_use( r->stack_ptr[-1] );
                ptr += 2 ;
                break ;

            case MN_GET_REMOTE_PUBLIC | MN_STRING:
                i = instance_get( r->stack_ptr[-1] );
                if ( !i )
                {
                    fprintf( stderr, "ERROR: Runtime error in %s(%d) - Process %d not active\n", r->proc->name, LOCDWORD( r, PROCESS_ID ), r->stack_ptr[-1] ) ;
                    exit( 0 );
                }
                else
                    r->stack_ptr[-1] = PUBDWORD( i, ptr[1] ) ;
                string_use( r->stack_ptr[-1] );
                ptr += 2 ;
                break ;

            case MN_STRING | MN_PTR:
                r->stack_ptr[-1] = *( int32_t * )r->stack_ptr[-1] ;
                string_use( r->stack_ptr[-1] );
                ptr++ ;
                break ;

            case MN_STRING | MN_POP:
                string_discard( r->stack_ptr[-1] ) ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

                /* Acceso a variables tipo WORD */

            case MN_WORD | MN_GET_PRIV:
                *r->stack_ptr++ = PRIINT16( r, ptr[1] ) ;
                ptr += 2 ;
                break ;

            case MN_WORD | MN_GET_PRIV | MN_UNSIGNED:
                *r->stack_ptr++ = PRIWORD( r, ptr[1] ) ;
                ptr += 2 ;
                break ;

            case MN_WORD | MN_GET_PUBLIC:
                *r->stack_ptr++ = PUBINT16( r, ptr[1] ) ;
                ptr += 2 ;
                break ;

            case MN_WORD | MN_GET_PUBLIC | MN_UNSIGNED:
                *r->stack_ptr++ = PUBWORD( r, ptr[1] ) ;
                ptr += 2 ;
                break ;

            case MN_WORD | MN_GET_LOCAL:
                *r->stack_ptr++ = LOCINT16( r, ptr[1] ) ;
                ptr += 2 ;
                break ;

            case MN_WORD | MN_GET_LOCAL | MN_UNSIGNED:
                *r->stack_ptr++ = LOCWORD( r, ptr[1] ) ;
                ptr += 2 ;
                break ;

            case MN_WORD | MN_GET_GLOBAL:
                *r->stack_ptr++ = GLOINT16( ptr[1] ) ;
                ptr += 2 ;
                break ;

            case MN_WORD | MN_GET_GLOBAL | MN_UNSIGNED:
                *r->stack_ptr++ = GLOWORD( ptr[1] ) ;
                ptr += 2 ;
                break ;

            case MN_WORD | MN_GET_REMOTE:
                i = instance_get( r->stack_ptr[-1] ) ;
                if ( !i )
                {
                    fprintf( stderr, "ERROR: Runtime error in %s(%d) - Process %d not active\n", r->proc->name, LOCDWORD( r, PROCESS_ID ), r->stack_ptr[-1] ) ;
                    exit( 0 );
                }
                else
                    r->stack_ptr[-1] = LOCINT16( i, ptr[1] ) ;
                ptr += 2 ;
                break ;

            case MN_WORD | MN_GET_REMOTE | MN_UNSIGNED:
                i = instance_get( r->stack_ptr[-1] ) ;
                if ( !i )
                {
                    fprintf( stderr, "ERROR: Runtime error in %s(%d) - Process %d not active\n", r->proc->name, LOCDWORD( r, PROCESS_ID ), r->stack_ptr[-1] ) ;
                    exit( 0 );
                }
                else
                    r->stack_ptr[-1] = LOCWORD( i, ptr[1] ) ;
                ptr += 2 ;
                break ;

            case MN_WORD | MN_GET_REMOTE_PUBLIC:
                i = instance_get( r->stack_ptr[-1] ) ;
                if ( !i )
                {
                    fprintf( stderr, "ERROR: Runtime error in %s(%d) - Process %d not active\n", r->proc->name, LOCDWORD( r, PROCESS_ID ), r->stack_ptr[-1] ) ;
                    exit( 0 );
                }
                else
                    r->stack_ptr[-1] = PUBINT16( i, ptr[1] ) ;
                ptr += 2 ;
                break ;

            case MN_WORD | MN_GET_REMOTE_PUBLIC | MN_UNSIGNED:
                i = instance_get( r->stack_ptr[-1] ) ;
                if ( !i )
                {
                    fprintf( stderr, "ERROR: Runtime error in %s(%d) - Process %d not active\n", r->proc->name, LOCDWORD( r, PROCESS_ID ), r->stack_ptr[-1] ) ;
                    exit( 0 );
                }
                else
                    r->stack_ptr[-1] = PUBWORD( i, ptr[1] ) ;
                ptr += 2 ;
                break ;

            case MN_WORD | MN_PTR:
                r->stack_ptr[-1] = *( int16_t * )r->stack_ptr[-1] ;
                ptr++ ;
                break ;

            case MN_WORD | MN_PTR | MN_UNSIGNED:
                r->stack_ptr[-1] = *( uint16_t * )r->stack_ptr[-1] ;
                ptr++ ;
                break ;

                /* Acceso a variables tipo BYTE */

            case MN_BYTE | MN_GET_PRIV:
                *r->stack_ptr++ = PRIINT8( r, ptr[1] ) ;
                ptr += 2 ;
                break ;

            case MN_BYTE | MN_GET_PRIV | MN_UNSIGNED:
                *r->stack_ptr++ = PRIBYTE( r, ptr[1] ) ;
                ptr += 2 ;
                break ;

            case MN_BYTE | MN_GET_PUBLIC:
                *r->stack_ptr++ = PUBINT8( r, ptr[1] ) ;
                ptr += 2 ;
                break ;

            case MN_BYTE | MN_GET_PUBLIC | MN_UNSIGNED:
                *r->stack_ptr++ = PUBBYTE( r, ptr[1] ) ;
                ptr += 2 ;
                break ;

            case MN_BYTE | MN_GET_LOCAL:
                *r->stack_ptr++ = LOCINT8( r, ptr[1] ) ;
                ptr += 2 ;
                break ;

            case MN_BYTE | MN_GET_LOCAL | MN_UNSIGNED:
                *r->stack_ptr++ = LOCBYTE( r, ptr[1] ) ;
                ptr += 2 ;
                break ;

            case MN_BYTE | MN_GET_GLOBAL:
                *r->stack_ptr++ = GLOINT8( ptr[1] ) ;
                ptr += 2 ;
                break ;

            case MN_BYTE | MN_GET_GLOBAL | MN_UNSIGNED:
                *r->stack_ptr++ = GLOBYTE( ptr[1] ) ;
                ptr += 2 ;
                break ;

            case MN_BYTE | MN_GET_REMOTE:
                i = instance_get( r->stack_ptr[-1] ) ;
                if ( !i )
                {
                    fprintf( stderr, "ERROR: Runtime error in %s(%d) - Process %d not active\n", r->proc->name, LOCDWORD( r, PROCESS_ID ), r->stack_ptr[-1] ) ;
                    exit( 0 );
                }
                else
                    r->stack_ptr[-1] = LOCINT8( i, ptr[1] ) ;
                ptr += 2 ;
                break ;

            case MN_BYTE | MN_GET_REMOTE | MN_UNSIGNED:
                i = instance_get( r->stack_ptr[-1] ) ;
                if ( !i )
                {
                    fprintf( stderr, "ERROR: Runtime error in %s(%d) - Process %d not active\n", r->proc->name, LOCDWORD( r, PROCESS_ID ), r->stack_ptr[-1] ) ;
                    exit( 0 );
                }
                else
                    r->stack_ptr[-1] = LOCBYTE( i, ptr[1] ) ;
                ptr += 2 ;
                break ;

            case MN_BYTE | MN_GET_REMOTE_PUBLIC:
                i = instance_get( r->stack_ptr[-1] ) ;
                if ( !i )
                {
                    fprintf( stderr, "ERROR: Runtime error in %s(%d) - Process %d not active\n", r->proc->name, LOCDWORD( r, PROCESS_ID ), r->stack_ptr[-1] ) ;
                    exit( 0 );
                }
                else
                    r->stack_ptr[-1] = PUBINT8( i, ptr[1] ) ;
                ptr += 2 ;
                break ;

            case MN_BYTE | MN_GET_REMOTE_PUBLIC | MN_UNSIGNED:
                i = instance_get( r->stack_ptr[-1] ) ;
                if ( !i )
                {
                    fprintf( stderr, "ERROR: Runtime error in %s(%d) - Process %d not active\n", r->proc->name, LOCDWORD( r, PROCESS_ID ), r->stack_ptr[-1] ) ;
                    exit( 0 );
                }
                else
                    r->stack_ptr[-1] = PUBBYTE( i, ptr[1] ) ;
                ptr += 2 ;
                break ;

            case MN_BYTE | MN_PTR:
                r->stack_ptr[-1] = *(( int8_t * )r->stack_ptr[-1] ) ;
                ptr++ ;
                break ;

            case MN_BYTE | MN_PTR | MN_UNSIGNED:
                r->stack_ptr[-1] = *(( uint8_t * )r->stack_ptr[-1] ) ;
                ptr++ ;
                break ;

                /*            case MN_BYTE | MN_NOT:
                            case MN_BYTE | MN_NOT | MN_UNSIGNED:
                                r->stack_ptr[-1] = !(r->stack_ptr[-1]) ;
                                ptr++ ;
                                break ; */

                /* Operaciones matemáticas  en coma floatante */

            case MN_FLOAT | MN_NEG:
                *( float * )&r->stack_ptr[-1] = -*(( float * ) & r->stack_ptr[-1] ) ;
                ptr++ ;
                break ;

            case MN_FLOAT | MN_NOT:
                *( float * )&r->stack_ptr[-1] = ( float ) !*(( float * ) & r->stack_ptr[-1] ) ;
                ptr++ ;
                break ;

            case MN_FLOAT | MN_ADD:
                *( float * )&r->stack_ptr[-2] += *(( float * ) & r->stack_ptr[-1] ) ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_FLOAT | MN_SUB:
                *( float * )&r->stack_ptr[-2] -= *(( float * ) & r->stack_ptr[-1] ) ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_FLOAT | MN_MUL:
                *( float * )&r->stack_ptr[-2] *= *(( float * ) & r->stack_ptr[-1] ) ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_FLOAT | MN_DIV:
/*
                if ( *(( float * )&r->stack_ptr[-1] ) == 0.0 )
                {
                    fprintf( stderr, "ERROR: Runtime error in %s(%d) - Division by zero\n", r->proc->name, LOCDWORD( r, PROCESS_ID ) ) ;
                    exit( 0 );
                }
*/
                *( float * )&r->stack_ptr[-2] /= *(( float * ) & r->stack_ptr[-1] ) ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_FLOAT2INT:
                *( int32_t * )&( r->stack_ptr[-ptr[1] - 1] ) = ( int32_t ) * ( float * ) & ( r->stack_ptr[-ptr[1] - 1] ) ;
                ptr += 2 ;
                break ;

            case MN_INT2FLOAT:
            case MN_INT2FLOAT | MN_UNSIGNED:
                *( float * )&( r->stack_ptr[-ptr[1] - 1] ) = ( float ) * ( int32_t * ) & ( r->stack_ptr[-ptr[1] - 1] ) ;
                ptr += 2 ;
                break ;

            case MN_INT2WORD:
            case MN_INT2WORD | MN_UNSIGNED:
                *( uint32_t * )&( r->stack_ptr[-ptr[1] - 1] ) = ( int32_t )( uint16_t ) * ( int32_t * ) & ( r->stack_ptr[-ptr[1] - 1] ) ;
                ptr += 2;
                break;
            case MN_INT2BYTE:
            case MN_INT2BYTE | MN_UNSIGNED:
                *( uint32_t * )&( r->stack_ptr[-ptr[1] - 1] ) = ( int32_t )( uint8_t ) * ( int32_t * ) & ( r->stack_ptr[-ptr[1] - 1] ) ;
                ptr += 2;
                break;

                /* Operaciones matemáticas */

            case MN_NEG:
            case MN_NEG | MN_UNSIGNED:
                r->stack_ptr[-1] = -r->stack_ptr[-1] ;
                ptr++ ;
                break ;

            case MN_NOT:
            case MN_NOT | MN_UNSIGNED:
                r->stack_ptr[-1] = !( r->stack_ptr[-1] ) ;
                ptr++ ;
                break ;

            case MN_ADD:
                r->stack_ptr[-2] += r->stack_ptr[-1] ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_SUB:
                r->stack_ptr[-2] -= r->stack_ptr[-1] ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_MUL | MN_WORD:
            case MN_MUL | MN_BYTE:
            case MN_MUL:
                r->stack_ptr[-2] *= r->stack_ptr[-1] ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_MUL | MN_WORD | MN_UNSIGNED:
            case MN_MUL | MN_BYTE | MN_UNSIGNED:
            case MN_MUL | MN_UNSIGNED:
                r->stack_ptr[-2] = ( uint32_t )r->stack_ptr[-2] * ( uint32_t )r->stack_ptr[-1] ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_DIV | MN_WORD:
            case MN_DIV | MN_BYTE:
            case MN_DIV:
                if ( r->stack_ptr[-1] == 0 )
                {
                    fprintf( stderr, "ERROR: Runtime error in %s(%d) - Division by zero\n", r->proc->name, LOCDWORD( r, PROCESS_ID ) ) ;
                    exit( 0 );
                }
                r->stack_ptr[-2] /= r->stack_ptr[-1] ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_DIV | MN_WORD | MN_UNSIGNED:
            case MN_DIV | MN_BYTE | MN_UNSIGNED:
            case MN_DIV | MN_UNSIGNED:
                if ( r->stack_ptr[-1] == 0 )
                {
                    fprintf( stderr, "ERROR: Runtime error in %s(%d) - Division by zero\n", r->proc->name, LOCDWORD( r, PROCESS_ID ) ) ;
                    exit( 0 );
                }
                r->stack_ptr[-2] = ( uint32_t )r->stack_ptr[-2] / ( uint32_t )r->stack_ptr[-1] ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_MOD | MN_WORD:
            case MN_MOD | MN_BYTE:
            case MN_MOD:
                if ( r->stack_ptr[-1] == 0 )
                {
                    fprintf( stderr, "ERROR: Runtime error in %s(%d) - Division by zero\n", r->proc->name, LOCDWORD( r, PROCESS_ID ) ) ;
                    exit( 0 );
                }
                r->stack_ptr[-2] %= r->stack_ptr[-1] ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_MOD | MN_WORD | MN_UNSIGNED:
            case MN_MOD | MN_BYTE | MN_UNSIGNED:
            case MN_MOD | MN_UNSIGNED:
                if ( r->stack_ptr[-1] == 0 )
                {
                    fprintf( stderr, "ERROR: Runtime error in %s(%d) - Division by zero\n", r->proc->name, LOCDWORD( r, PROCESS_ID ) ) ;
                    exit( 0 );
                }
                r->stack_ptr[-2] = ( uint32_t )r->stack_ptr[-2] % ( uint32_t )r->stack_ptr[-1] ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

                /* Operaciones a nivel de bit */

            case MN_ROR:
                ( r->stack_ptr[-2] ) = (( int32_t )r->stack_ptr[-2] ) >> r->stack_ptr[-1] ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_ROR | MN_UNSIGNED:
                r->stack_ptr[-2] = (( uint32_t ) r->stack_ptr[-2] ) >> r->stack_ptr[-1] ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_WORD | MN_ROR:
                r->stack_ptr[-2] = (( int16_t ) r->stack_ptr[-2] ) >> r->stack_ptr[-1] ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_WORD | MN_ROR | MN_UNSIGNED:
                r->stack_ptr[-2] = (( uint16_t ) r->stack_ptr[-2] ) >> r->stack_ptr[-1] ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_BYTE | MN_ROR:
                r->stack_ptr[-2] = (( int8_t ) r->stack_ptr[-2] >> r->stack_ptr[-1] ) ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_BYTE | MN_ROR | MN_UNSIGNED:
                r->stack_ptr[-2] = (( uint8_t ) r->stack_ptr[-2] ) >> r->stack_ptr[-1] ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_ROL:
                ( r->stack_ptr[-2] ) = (( int32_t )r->stack_ptr[-2] ) << r->stack_ptr[-1] ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

                /* Todos estos ROL siguientes no serian necesarios, pero bueno... */
            case MN_ROL | MN_UNSIGNED:
                ( r->stack_ptr[-2] ) = ( uint32_t )( r->stack_ptr[-2] << r->stack_ptr[-1] );
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_WORD | MN_ROL:
                ( r->stack_ptr[-2] ) = (( int16_t )r->stack_ptr[-2] ) << r->stack_ptr[-1];
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_WORD | MN_ROL | MN_UNSIGNED:
                ( r->stack_ptr[-2] ) = ( uint16_t )( r->stack_ptr[-2] << r->stack_ptr[-1] );
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_BYTE | MN_ROL:
                ( r->stack_ptr[-2] ) = (( int8_t )r->stack_ptr[-2] ) << r->stack_ptr[-1];
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_BYTE | MN_ROL | MN_UNSIGNED:
                ( r->stack_ptr[-2] ) = ( uint8_t )( r->stack_ptr[-2] << r->stack_ptr[-1] );
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_BAND:
            case MN_BAND | MN_UNSIGNED:
                r->stack_ptr[-2] &= r->stack_ptr[-1] ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_BOR:
            case MN_BOR | MN_UNSIGNED:
                r->stack_ptr[-2] |= r->stack_ptr[-1] ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_BXOR:
            case MN_BXOR | MN_UNSIGNED:
                r->stack_ptr[-2] ^= r->stack_ptr[-1] ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_BNOT:
            case MN_BNOT | MN_UNSIGNED:
                r->stack_ptr[-1] = ~( r->stack_ptr[-1] ) ;
                ptr++ ;
                break ;

            case MN_BYTE | MN_BNOT:
                r->stack_ptr[-1] = ( int8_t ) ~( r->stack_ptr[-1] ) ;
                ptr++ ;
                break ;

            case MN_BYTE | MN_BNOT | MN_UNSIGNED:
                r->stack_ptr[-1] = ( uint8_t ) ~( r->stack_ptr[-1] ) ;
                ptr++ ;
                break ;

            case MN_WORD | MN_BNOT:
                r->stack_ptr[-1] = ( int16_t ) ~( r->stack_ptr[-1] ) ;
                ptr++ ;
                break ;

            case MN_WORD | MN_BNOT | MN_UNSIGNED:
                r->stack_ptr[-1] = ( uint16_t ) ~( r->stack_ptr[-1] ) ;
                ptr++ ;
                break ;

                /* Operaciones logicas */

            case MN_AND:
                r->stack_ptr[-2] = r->stack_ptr[-2] && r->stack_ptr[-1] ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_OR:
                r->stack_ptr[-2] = r->stack_ptr[-2] || r->stack_ptr[-1] ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_XOR:
                r->stack_ptr[-2] = ( r->stack_ptr[-2] != 0 ) ^( r->stack_ptr[-1] != 0 ) ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

                /* Comparaciones */

            case MN_EQ:
                r->stack_ptr[-2] = ( r->stack_ptr[-2] == r->stack_ptr[-1] ) ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_NE:
                r->stack_ptr[-2] = ( r->stack_ptr[-2] != r->stack_ptr[-1] ) ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_GTE:
                r->stack_ptr[-2] = ( r->stack_ptr[-2] >= r->stack_ptr[-1] ) ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_GTE | MN_UNSIGNED:
                r->stack_ptr[-2] = (( uint32_t )r->stack_ptr[-2] >= ( uint32_t )r->stack_ptr[-1] ) ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_LTE:
                r->stack_ptr[-2] = ( r->stack_ptr[-2] <= r->stack_ptr[-1] ) ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_LTE | MN_UNSIGNED:
                r->stack_ptr[-2] = (( uint32_t )r->stack_ptr[-2] <= ( uint32_t )r->stack_ptr[-1] ) ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_LT:
                r->stack_ptr[-2] = ( r->stack_ptr[-2] < r->stack_ptr[-1] ) ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_LT | MN_UNSIGNED:
                r->stack_ptr[-2] = (( uint32_t )r->stack_ptr[-2] < ( uint32_t )r->stack_ptr[-1] ) ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_GT:
                r->stack_ptr[-2] = ( r->stack_ptr[-2] > r->stack_ptr[-1] ) ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_GT | MN_UNSIGNED:
                r->stack_ptr[-2] = (( uint32_t )r->stack_ptr[-2] > ( uint32_t )r->stack_ptr[-1] ) ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

                /* Comparaciones con floats */

            case MN_EQ | MN_FLOAT:
                r->stack_ptr[-2] = ( *( float * ) & r->stack_ptr[-2] == *( float * ) & r->stack_ptr[-1] ) ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_NE | MN_FLOAT:
                r->stack_ptr[-2] = ( *( float * ) & r->stack_ptr[-2] != *( float * ) & r->stack_ptr[-1] ) ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_GTE | MN_FLOAT:
                r->stack_ptr[-2] = ( *( float * ) & r->stack_ptr[-2] >= *( float * ) & r->stack_ptr[-1] ) ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_LTE | MN_FLOAT:
                r->stack_ptr[-2] = ( *( float * ) & r->stack_ptr[-2] <= *( float * ) & r->stack_ptr[-1] ) ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_LT | MN_FLOAT:
                r->stack_ptr[-2] = ( *( float * ) & r->stack_ptr[-2] < *( float * ) & r->stack_ptr[-1] ) ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_GT | MN_FLOAT:
                r->stack_ptr[-2] = ( *( float * ) & r->stack_ptr[-2] > *( float * ) & r->stack_ptr[-1] ) ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

                /* Comparaciones con cadenas */

            case MN_EQ | MN_STRING :
                n = string_comp( r->stack_ptr[-2], r->stack_ptr[-1] ) == 0 ;
                string_discard( r->stack_ptr[-2] );
                string_discard( r->stack_ptr[-1] );
                r->stack_ptr[-2] = n;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_NE | MN_STRING :
                n = string_comp( r->stack_ptr[-2], r->stack_ptr[-1] ) != 0 ;
                string_discard( r->stack_ptr[-2] );
                string_discard( r->stack_ptr[-1] );
                r->stack_ptr[-2] = n;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_GTE | MN_STRING :
                n = string_comp( r->stack_ptr[-2], r->stack_ptr[-1] ) >= 0 ;
                string_discard( r->stack_ptr[-2] );
                string_discard( r->stack_ptr[-1] );
                r->stack_ptr[-2] = n;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_LTE | MN_STRING :
                n = string_comp( r->stack_ptr[-2], r->stack_ptr[-1] ) <= 0 ;
                string_discard( r->stack_ptr[-2] );
                string_discard( r->stack_ptr[-1] );
                r->stack_ptr[-2] = n;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_LT | MN_STRING :
                n = string_comp( r->stack_ptr[-2], r->stack_ptr[-1] ) <  0 ;
                string_discard( r->stack_ptr[-2] );
                string_discard( r->stack_ptr[-1] );
                r->stack_ptr[-2] = n;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_GT | MN_STRING :
                n = string_comp( r->stack_ptr[-2], r->stack_ptr[-1] ) >  0 ;
                string_discard( r->stack_ptr[-2] );
                string_discard( r->stack_ptr[-1] );
                r->stack_ptr[-2] = n;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

                /* Operaciones con cadenas */

            case MN_VARADD | MN_STRING:
                n = *( int32_t * )( r->stack_ptr[-2] ) ;
                *( int32_t * )( r->stack_ptr[-2] ) = string_add( n, r->stack_ptr[-1] ) ;
                string_use( *( int32_t * )( r->stack_ptr[-2] ) );
                string_discard( n );
                string_discard( r->stack_ptr[-1] );
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_LETNP | MN_STRING:
                string_discard( *( int32_t * )( r->stack_ptr[-2] ) );
                ( *( int32_t * )( r->stack_ptr[-2] ) ) = r->stack_ptr[-1];
                r->stack_ptr -= 2 ;
                ptr++ ;
                break ;

            case MN_LET | MN_STRING:
                string_discard( *( int32_t * )( r->stack_ptr[-2] ) );
                ( *( int32_t * )( r->stack_ptr[-2] ) ) = r->stack_ptr[-1];
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_ADD | MN_STRING:
                n = string_add( r->stack_ptr[-2], r->stack_ptr[-1] );
                string_use( n ) ;
                string_discard( r->stack_ptr[-2] );
                string_discard( r->stack_ptr[-1] );
                r->stack_ptr-- ;
                r->stack_ptr[-1] = n ;
                ptr++ ;
                break ;

            case MN_INT2STR:
                r->stack_ptr[-ptr[1] - 1] = string_itoa( r->stack_ptr[-ptr[1] - 1] ) ;
                string_use( r->stack_ptr[-ptr[1] - 1] ) ;
                ptr += 2 ;
                break ;

            case MN_INT2STR | MN_UNSIGNED:
                r->stack_ptr[-ptr[1] - 1] = string_uitoa( r->stack_ptr[-ptr[1] - 1] ) ;
                string_use( r->stack_ptr[-ptr[1] - 1] ) ;
                ptr += 2 ;
                break ;

            case MN_INT2STR | MN_WORD:
                r->stack_ptr[-ptr[1] - 1] = string_itoa( r->stack_ptr[-ptr[1] - 1] ) ;
                string_use( r->stack_ptr[-ptr[1] - 1] ) ;
                ptr += 2 ;
                break ;

            case MN_INT2STR | MN_UNSIGNED | MN_WORD:
                r->stack_ptr[-ptr[1] - 1] = string_uitoa( r->stack_ptr[-ptr[1] - 1] ) ;
                string_use( r->stack_ptr[-ptr[1] - 1] ) ;
                ptr += 2 ;
                break ;

            case MN_INT2STR | MN_BYTE:
                r->stack_ptr[-ptr[1] - 1] = string_itoa( r->stack_ptr[-ptr[1] - 1] ) ;
                string_use( r->stack_ptr[-ptr[1] - 1] ) ;
                ptr += 2 ;
                break ;

            case MN_INT2STR | MN_UNSIGNED | MN_BYTE:
                r->stack_ptr[-ptr[1] - 1] = string_uitoa( r->stack_ptr[-ptr[1] - 1] ) ;
                string_use( r->stack_ptr[-ptr[1] - 1] ) ;
                ptr += 2 ;
                break ;

            case MN_FLOAT2STR:
                r->stack_ptr[-ptr[1] - 1] = string_ftoa( *( float * ) & r->stack_ptr[-ptr[1] - 1] ) ;
                string_use( r->stack_ptr[-ptr[1] - 1] ) ;
                ptr += 2 ;
                break ;

            case MN_CHR2STR:
                buffer[0] = ( uint8_t )r->stack_ptr[-ptr[1] - 1] ;
                buffer[1] = 0 ;
                r->stack_ptr[-ptr[1] - 1] = string_new( buffer ) ;
                string_use( r->stack_ptr[-ptr[1] - 1] ) ;
                ptr += 2 ;
                break ;

            case MN_CHRSTR:
                n = string_char( r->stack_ptr[-2], r->stack_ptr[-1] ) ;
                string_use( n ) ;
                string_discard( r->stack_ptr[-2] );
                r->stack_ptr-- ;
                r->stack_ptr[-1] = n ;
                ptr++ ;
                break ;

            case MN_STR2CHR:
                n = r->stack_ptr[-ptr[1] - 1] ;
                r->stack_ptr[-1] = *string_get( n ) ;
                string_discard( n );
                ptr += 2 ;
                break ;

            case MN_POINTER2STR:
                r->stack_ptr[-ptr[1] - 1] = string_ptoa( *( void ** ) & r->stack_ptr[-ptr[1] - 1] ) ;
                string_use( r->stack_ptr[-ptr[1] - 1] ) ;
                ptr += 2 ;
                break ;

                /*
                case MN_POINTER2BOL:
                    r->stack_ptr[-ptr[1]-1] = (r->stack_ptr[-ptr[1]-1]) ? 1:0 ;
                    ptr += 2 ;
                    break ;
                */

            case MN_STR2FLOAT:
                n = r->stack_ptr[-ptr[1] - 1] ;
                str = ( char * )string_get( n ) ;
                *( float * )( &r->stack_ptr[-ptr[1] - 1] ) = str ? ( float )atof( str ) : 0.0f ;
                string_discard( n ) ;
                ptr += 2 ;
                break ;

            case MN_STR2INT:
                n = r->stack_ptr[-ptr[1] - 1] ;
                str = ( char * )string_get( n ) ;
                r->stack_ptr[-ptr[1] - 1] = str ? atoi( str ) : 0 ;
                string_discard( n ) ;
                ptr += 2 ;
                break ;

                /* Operaciones con cadenas de ancho fijo */

            case MN_A2STR:
                str = *( char ** )( &r->stack_ptr[-ptr[1] - 1] ) ;
                n = string_new( str );
                string_use( n );
                r->stack_ptr[-ptr[1] - 1] = n ;
                ptr += 2 ;
                break ;

            case MN_STR2A:
                n = r->stack_ptr[-1];
                strncpy( *( char ** )( &r->stack_ptr[-2] ), string_get( n ), ptr[1] ) ;
                (( char * )( r->stack_ptr[-2] ) )[ptr[1]] = 0;
                r->stack_ptr[-2] = r->stack_ptr[-1];
                r->stack_ptr--;
                ptr += 2 ;
                break ;

            case MN_STRACAT:
                n = r->stack_ptr[-1];
                strncat( *( char ** )( &r->stack_ptr[-2] ), string_get( n ), ptr[1] - strlen( *( char ** )( &r->stack_ptr[-2] ) ) ) ;
                (( char * )( r->stack_ptr[-2] ) )[ptr[1]] = 0;
                r->stack_ptr[-2] = r->stack_ptr[-1];
                r->stack_ptr--;
                ptr += 2 ;
                break ;

                /* Operaciones directas con variables tipo DWORD */

            case MN_LETNP:
            case MN_LETNP | MN_UNSIGNED:
                ( *( int32_t * )( r->stack_ptr[-2] ) ) = r->stack_ptr[-1] ;
                r->stack_ptr -= 2 ;
                ptr++ ;
                break ;

            case MN_LET:
            case MN_LET | MN_UNSIGNED:
                ( *( int32_t * )( r->stack_ptr[-2] ) ) = r->stack_ptr[-1] ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_INC:
            case MN_INC | MN_UNSIGNED:
                ( *( int32_t * )( r->stack_ptr[-1] ) ) += ptr[1] ;
                ptr += 2 ;
                break ;

            case MN_DEC:
            case MN_DEC | MN_UNSIGNED:
                ( *( int32_t * )( r->stack_ptr[-1] ) ) -= ptr[1] ;
                ptr += 2 ;
                break ;

            case MN_POSTDEC:
            case MN_POSTDEC | MN_UNSIGNED:
                ( *( int32_t * )( r->stack_ptr[-1] ) ) -= ptr[1] ;
                r->stack_ptr[-1] = *( int32_t * )( r->stack_ptr[-1] ) + ptr[1] ;
                ptr += 2 ;
                break ;

            case MN_POSTINC:
            case MN_POSTINC | MN_UNSIGNED:
                *(( int32_t * )( r->stack_ptr[-1] ) ) += ptr[1] ;
                r->stack_ptr[-1] = *( int32_t * )( r->stack_ptr[-1] ) - ptr[1] ;
                ptr += 2 ;
                break ;

            case MN_VARADD:
            case MN_VARADD | MN_UNSIGNED:
                *( int32_t * )( r->stack_ptr[-2] ) += r->stack_ptr[-1] ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_VARSUB:
            case MN_VARSUB | MN_UNSIGNED:
                *( int32_t * )( r->stack_ptr[-2] ) -= r->stack_ptr[-1] ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_VARMUL:
            case MN_VARMUL | MN_UNSIGNED:
                *( int32_t * )( r->stack_ptr[-2] ) *= r->stack_ptr[-1] ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_VARDIV:
            case MN_VARDIV | MN_UNSIGNED:
                if ( r->stack_ptr[-1] == 0 )
                {
                    fprintf( stderr, "ERROR: Runtime error in %s(%d) - Division by zero\n", r->proc->name, LOCDWORD( r, PROCESS_ID ) ) ;
                    exit( 0 );
                }
                *( int32_t * )( r->stack_ptr[-2] ) /= r->stack_ptr[-1] ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_VARMOD:
            case MN_VARMOD | MN_UNSIGNED:
                if ( r->stack_ptr[-1] == 0 )
                {
                    fprintf( stderr, "ERROR: Runtime error in %s(%d) - Division by zero\n", r->proc->name, LOCDWORD( r, PROCESS_ID ) ) ;
                    exit( 0 );
                }
                *( int32_t * )( r->stack_ptr[-2] ) %= r->stack_ptr[-1] ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_VAROR:
            case MN_VAROR | MN_UNSIGNED:
                *( int32_t * )( r->stack_ptr[-2] ) |= r->stack_ptr[-1] ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_VARXOR:
            case MN_VARXOR | MN_UNSIGNED:
                *( int32_t * )( r->stack_ptr[-2] ) ^= r->stack_ptr[-1] ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_VARAND:
            case MN_VARAND | MN_UNSIGNED:
                *( int32_t * )( r->stack_ptr[-2] ) &= r->stack_ptr[-1] ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_VARROR:
                *( int32_t * )( r->stack_ptr[-2] ) >>= r->stack_ptr[-1] ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_VARROR | MN_UNSIGNED:
                *( uint32_t * )( r->stack_ptr[-2] ) >>= r->stack_ptr[-1] ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_VARROL:
                *( int32_t * )( r->stack_ptr[-2] ) <<= r->stack_ptr[-1] ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_VARROL | MN_UNSIGNED:
                *( uint32_t * )( r->stack_ptr[-2] ) <<= r->stack_ptr[-1] ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

                /* Operaciones directas con variables tipo WORD */

            case MN_WORD | MN_LETNP:
            case MN_WORD | MN_LETNP | MN_UNSIGNED:
                ( *( int16_t * )( r->stack_ptr[-2] ) ) = r->stack_ptr[-1] ;
                r->stack_ptr -= 2 ;
                ptr++ ;
                break ;

            case MN_WORD | MN_LET:
            case MN_WORD | MN_LET | MN_UNSIGNED:
                ( *( int16_t * )( r->stack_ptr[-2] ) ) = r->stack_ptr[-1] ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_WORD | MN_INC:
            case MN_WORD | MN_INC | MN_UNSIGNED:
                ( *( int16_t * )( r->stack_ptr[-1] ) ) += ptr[1] ;
                ptr += 2 ;
                break ;

            case MN_WORD | MN_DEC:
            case MN_WORD | MN_DEC | MN_UNSIGNED:
                ( *( int16_t * )( r->stack_ptr[-1] ) ) -= ptr[1] ;
                ptr += 2 ;
                break ;

            case MN_WORD | MN_POSTDEC:
            case MN_WORD | MN_POSTDEC | MN_UNSIGNED:
                ( *( int16_t * )( r->stack_ptr[-1] ) ) -= ptr[1] ;
                r->stack_ptr[-1] = *( int16_t * )( r->stack_ptr[-1] ) + ptr[1] ;
                ptr += 2 ;
                break ;

            case MN_WORD | MN_POSTINC:
            case MN_WORD | MN_POSTINC | MN_UNSIGNED:
                *(( int16_t * )( r->stack_ptr[-1] ) ) += ptr[1] ;
                r->stack_ptr[-1] = *( int16_t * )( r->stack_ptr[-1] ) - ptr[1] ;
                ptr += 2 ;
                break ;

            case MN_WORD | MN_VARADD:
            case MN_WORD | MN_VARADD | MN_UNSIGNED:
                *( int16_t * )( r->stack_ptr[-2] ) += r->stack_ptr[-1] ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_WORD | MN_VARSUB:
            case MN_WORD | MN_VARSUB | MN_UNSIGNED:
                *( int16_t * )( r->stack_ptr[-2] ) -= r->stack_ptr[-1] ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_WORD | MN_VARMUL:
            case MN_WORD | MN_VARMUL | MN_UNSIGNED:
                *( int16_t * )( r->stack_ptr[-2] ) *= r->stack_ptr[-1] ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_WORD | MN_VARDIV:
            case MN_WORD | MN_VARDIV | MN_UNSIGNED:
                if (( int16_t )r->stack_ptr[-1] == 0 )
                {
                    fprintf( stderr, "ERROR: Runtime error in %s(%d) - Division by zero\n", r->proc->name, LOCDWORD( r, PROCESS_ID ) ) ;
                    exit( 0 );
                }
                *( int16_t * )( r->stack_ptr[-2] ) /= ( int16_t )r->stack_ptr[-1] ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_WORD | MN_VARMOD:
            case MN_WORD | MN_VARMOD | MN_UNSIGNED:
                if (( int16_t )r->stack_ptr[-1] == 0 )
                {
                    fprintf( stderr, "ERROR: Runtime error in %s(%d) - Division by zero\n", r->proc->name, LOCDWORD( r, PROCESS_ID ) ) ;
                    exit( 0 );
                }
                *( int16_t * )( r->stack_ptr[-2] ) %= ( int16_t )r->stack_ptr[-1] ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_WORD | MN_VAROR:
            case MN_WORD | MN_VAROR | MN_UNSIGNED:
                *( int16_t * )( r->stack_ptr[-2] ) |= r->stack_ptr[-1] ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_WORD | MN_VARXOR:
            case MN_WORD | MN_VARXOR | MN_UNSIGNED:
                *( int16_t * )( r->stack_ptr[-2] ) ^= r->stack_ptr[-1] ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_WORD | MN_VARAND:
            case MN_WORD | MN_VARAND | MN_UNSIGNED:
                *( int16_t * )( r->stack_ptr[-2] ) &= r->stack_ptr[-1] ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_WORD | MN_VARROR:
                *( int16_t * )( r->stack_ptr[-2] ) >>= r->stack_ptr[-1] ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_WORD | MN_VARROR | MN_UNSIGNED:
                *( uint16_t * )( r->stack_ptr[-2] ) >>= r->stack_ptr[-1] ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_WORD | MN_VARROL:
                *( int16_t * )( r->stack_ptr[-2] ) <<= r->stack_ptr[-1] ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_WORD | MN_VARROL | MN_UNSIGNED:
                *( uint16_t * )( r->stack_ptr[-2] ) <<= r->stack_ptr[-1] ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

                /* Operaciones directas con variables tipo BYTE */

            case MN_BYTE | MN_LETNP:
            case MN_BYTE | MN_LETNP | MN_UNSIGNED:
                ( *( uint8_t * )( r->stack_ptr[-2] ) ) = r->stack_ptr[-1] ;
                r->stack_ptr -= 2 ;
                ptr++ ;
                break ;

            case MN_BYTE | MN_LET:
            case MN_BYTE | MN_LET | MN_UNSIGNED:
                ( *( uint8_t * )( r->stack_ptr[-2] ) ) = r->stack_ptr[-1] ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_BYTE | MN_INC:
            case MN_BYTE | MN_INC | MN_UNSIGNED:
                ( *( uint8_t * )( r->stack_ptr[-1] ) ) += ptr[1] ;
                ptr += 2 ;
                break ;

            case MN_BYTE | MN_DEC:
            case MN_BYTE | MN_DEC | MN_UNSIGNED:
                ( *( uint8_t * )( r->stack_ptr[-1] ) ) -= ptr[1] ;
                ptr += 2 ;
                break ;

            case MN_BYTE | MN_POSTDEC:
            case MN_BYTE | MN_POSTDEC | MN_UNSIGNED:
                ( *( uint8_t * )( r->stack_ptr[-1] ) ) -= ptr[1] ;
                r->stack_ptr[-1] = *( uint8_t * )( r->stack_ptr[-1] ) + ptr[1] ;
                ptr += 2 ;
                break ;

            case MN_BYTE | MN_POSTINC:
            case MN_BYTE | MN_POSTINC | MN_UNSIGNED:
                *(( uint8_t * )( r->stack_ptr[-1] ) ) += ptr[1] ;
                r->stack_ptr[-1] = *( uint8_t * )( r->stack_ptr[-1] ) - ptr[1] ;
                ptr += 2 ;
                break ;

            case MN_BYTE | MN_VARADD:
            case MN_BYTE | MN_VARADD | MN_UNSIGNED:
                *( uint8_t * )( r->stack_ptr[-2] ) += r->stack_ptr[-1] ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_BYTE | MN_VARSUB:
            case MN_BYTE | MN_VARSUB | MN_UNSIGNED:
                *( uint8_t * )( r->stack_ptr[-2] ) -= r->stack_ptr[-1] ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_BYTE | MN_VARMUL:
            case MN_BYTE | MN_VARMUL | MN_UNSIGNED:
                *( uint8_t * )( r->stack_ptr[-2] ) *= r->stack_ptr[-1] ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_BYTE | MN_VARDIV:
            case MN_BYTE | MN_VARDIV | MN_UNSIGNED:
                if (( uint8_t )r->stack_ptr[-1] == 0 )
                {
                    fprintf( stderr, "ERROR: Runtime error in %s(%d) - Division by zero\n", r->proc->name, LOCDWORD( r, PROCESS_ID ) ) ;
                    exit( 0 );
                }
                *( uint8_t * )( r->stack_ptr[-2] ) /= ( uint8_t )r->stack_ptr[-1] ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_BYTE | MN_VARMOD:
            case MN_BYTE | MN_VARMOD | MN_UNSIGNED:
                if (( uint8_t )r->stack_ptr[-1] == 0 )
                {
                    fprintf( stderr, "ERROR: Runtime error in %s(%d) - Division by zero\n", r->proc->name, LOCDWORD( r, PROCESS_ID ) ) ;
                    exit( 0 );
                }
                *( uint8_t * )( r->stack_ptr[-2] ) %= ( uint8_t )r->stack_ptr[-1] ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_BYTE | MN_VAROR:
            case MN_BYTE | MN_VAROR | MN_UNSIGNED:
                *( uint8_t * )( r->stack_ptr[-2] ) |= r->stack_ptr[-1] ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_BYTE | MN_VARXOR:
            case MN_BYTE | MN_VARXOR | MN_UNSIGNED:
                *( uint8_t * )( r->stack_ptr[-2] ) ^= r->stack_ptr[-1] ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_BYTE | MN_VARAND:
            case MN_BYTE | MN_VARAND | MN_UNSIGNED:
                *( uint8_t * )( r->stack_ptr[-2] ) &= r->stack_ptr[-1] ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_BYTE | MN_VARROR:
                *( int8_t * )( r->stack_ptr[-2] ) >>= r->stack_ptr[-1] ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_BYTE | MN_VARROR | MN_UNSIGNED:
                *( uint8_t * )( r->stack_ptr[-2] ) >>= r->stack_ptr[-1] ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_BYTE | MN_VARROL:
                *( int8_t * )( r->stack_ptr[-2] ) <<= r->stack_ptr[-1] ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_BYTE | MN_VARROL | MN_UNSIGNED:
                *( uint8_t * )( r->stack_ptr[-2] ) <<= r->stack_ptr[-1] ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

                /* Operaciones directas con variables tipo FLOAT */
            case MN_FLOAT | MN_LETNP:
                ( *( float * )( r->stack_ptr[-2] ) ) = *( float * ) & r->stack_ptr[-1] ;
                r->stack_ptr -= 2 ;
                ptr++ ;
                break ;

            case MN_FLOAT | MN_LET :
                ( *( float * )( r->stack_ptr[-2] ) ) = *( float * ) & r->stack_ptr[-1] ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_FLOAT | MN_INC:
                ( *( float * )( r->stack_ptr[-1] ) ) += ptr[1] ;
                ptr += 2 ;
                break ;

            case MN_FLOAT | MN_DEC:
                ( *( float * )( r->stack_ptr[-1] ) ) -= ptr[1] ;
                ptr += 2 ;
                break ;

            case MN_FLOAT | MN_POSTDEC:
                ( *( float * )( r->stack_ptr[-1] ) ) -= ptr[1] ;
                r->stack_ptr[-1] = *( uint32_t * )( r->stack_ptr[-1] ) + ptr[1] ;
                ptr += 2 ;
                break ;

            case MN_FLOAT | MN_POSTINC:
                *(( float * )( r->stack_ptr[-1] ) ) += ptr[1] ;
                r->stack_ptr[-1] = *( uint32_t * )( r->stack_ptr[-1] ) - ptr[1] ;
                ptr += 2 ;
                break ;

            case MN_FLOAT | MN_VARADD:
                *( float * )( r->stack_ptr[-2] ) += *( float * ) & r->stack_ptr[-1] ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_FLOAT | MN_VARSUB:
                *( float * )( r->stack_ptr[-2] ) -= *( float * ) & r->stack_ptr[-1] ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_FLOAT | MN_VARMUL:
                *( float * )( r->stack_ptr[-2] ) *= *( float * ) & r->stack_ptr[-1] ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

            case MN_FLOAT | MN_VARDIV:
/*
                if ( *( float * )&r->stack_ptr[-1] == 0.0 )
                {
                    fprintf( stderr, "ERROR: Runtime error in %s(%d) - Division by zero\n", r->proc->name, LOCDWORD( r, PROCESS_ID ) ) ;
                    exit( 0 );
                }
*/
                *( float * )( r->stack_ptr[-2] ) /= *( float * ) & r->stack_ptr[-1] ;
                r->stack_ptr-- ;
                ptr++ ;
                break ;

                /* Saltos */

            case MN_JUMP:
                ptr = r->code + ptr[1] ;
                continue ;

            case MN_JTRUE:
                r->stack_ptr-- ;
                if ( *r->stack_ptr )
                {
                    ptr = r->code + ptr[1] ;
                    continue ;
                }
                ptr += 2 ;
                break ;

            case MN_JFALSE:
                r->stack_ptr-- ;
                if ( !*r->stack_ptr )
                {
                    ptr = r->code + ptr[1] ;
                    continue ;
                }
                ptr += 2 ;
                break ;

            case MN_JTTRUE:
                if ( r->stack_ptr[-1] )
                {
                    ptr = r->code + ptr[1] ;
                    continue ;
                }
                ptr += 2 ;
                break ;

            case MN_JTFALSE:
                if ( !r->stack_ptr[-1] )
                {
                    ptr = r->code + ptr[1] ;
                    continue ;
                }
                ptr += 2 ;
                break ;

            case MN_NCALL:
                *r->stack_ptr++ = ptr - r->code + 2 ; // Push next address
                ptr = r->code + ptr[1] ; // Call function
                r->call_level++;
                break ;

                /* Switch */

            case MN_SWITCH:
                r->switchval = *--r->stack_ptr ;
                r->cased = 0 ;
                ptr++ ;
                break ;

            case MN_SWITCH | MN_STRING:
                if ( r->switchval_string != 0 ) string_discard( r->switchval_string );
                r->switchval_string = *--r->stack_ptr;
                r->cased = 0;
                ptr++;
                break;

            case MN_CASE:
                if ( r->switchval == *--r->stack_ptr ) r->cased = 2 ;
                ptr++ ;
                break ;

            case MN_CASE | MN_STRING:
                if ( string_comp( r->switchval_string, *--r->stack_ptr ) == 0 ) r->cased = 2 ;
                string_discard( *r->stack_ptr );
                string_discard( r->stack_ptr[-1] );
                ptr++;
                break;

            case MN_CASE_R:
                r->stack_ptr -= 2 ;
                if ( r->switchval >= r->stack_ptr[0] && r->switchval <= r->stack_ptr[1] ) r->cased = 1 ;
                ptr++ ;
                break ;

            case MN_CASE_R | MN_STRING:
                r->stack_ptr -= 2;
                if ( string_comp( r->switchval_string, r->stack_ptr[0] ) >= 0 &&
                        string_comp( r->switchval_string, r->stack_ptr[1] ) <= 0 )
                    r->cased = 1;
                string_discard( r->stack_ptr[0] );
                string_discard( r->stack_ptr[1] );
                ptr++;
                break;

            case MN_JNOCASE:
                if ( r->cased < 1 )
                {
                    ptr = r->code + ptr[1] ;
                    continue ;
                }
                ptr += 2 ;
                break ;

                /* Control de procesos */

            case MN_TYPE:
            {
                PROCDEF * proct = procdef_get( ptr[1] ) ;
                if ( !proct )
                {
                    fprintf( stderr, "ERROR: Runtime error in %s(%d) - Invalid type\n", r->proc->name, LOCDWORD( r, PROCESS_ID ) ) ;
                    exit( 0 );
                }
                *r->stack_ptr++ = proct->type ;
                ptr += 2 ;
                break ;
            }

            case MN_FRAME:
                LOCINT32( r, FRAME_PERCENT ) += r->stack_ptr[-1];
                r->stack_ptr-- ;
                r->codeptr = ptr + 1 ;
                return_value = LOCDWORD( r, PROCESS_ID );

                if ( !( r->proc->flags & PROC_FUNCTION ) &&
                        r->called_by && instance_exists( r->called_by ) && ( LOCDWORD( r->called_by, STATUS ) & STATUS_WAITING_MASK ) )
                {
                    /* We're returning and the parent is waiting: wake it up */
                    if ( r->called_by->stack && ( r->called_by->stack[0] & STACK_RETURN_VALUE ) )
                        r->called_by->stack_ptr[-1] = return_value;

                    LOCDWORD( r->called_by, STATUS ) &= ~STATUS_WAITING_MASK;
                    r->called_by = NULL;
                }
                goto break_all ;

            case MN_END:
                if ( r->call_level > 0 )
                {
                    ptr = r->code + *--r->stack_ptr ;
                    r->call_level--;
                    continue;
                }

                if ( LOCDWORD( r, STATUS ) != STATUS_DEAD ) LOCDWORD( r, STATUS ) = STATUS_KILLED ;
                goto break_all ;

            case MN_RETURN:
                if ( r->call_level > 0 )
                {
                    ptr = r->code + *--r->stack_ptr ;
                    r->call_level--;
                    continue;
                }

                if ( LOCDWORD( r, STATUS ) != STATUS_DEAD ) LOCDWORD( r, STATUS ) = STATUS_KILLED ;
                r->stack_ptr-- ;
                return_value = *r->stack_ptr ;
                goto break_all ;

                /* Handlers */

            case MN_EXITHNDLR:
                r->exitcode = ptr[1] ;
                ptr += 2 ;
                break;

            case MN_ERRHNDLR:
                r->errorcode = ptr[1] ;
                ptr += 2 ;
                break;

                /* Otros */

            case MN_DEBUG:
                if ( dcb.data.NSourceFiles )
                {
                    if ( debug ) printf( "\n----- DEBUG from %s(%d) -----\n", r->proc->name, LOCDWORD( r, PROCESS_ID ) ) ;
                    debug_next = 1;
                }
                ptr++;
                break ;

            case MN_SENTENCE:
                trace_sentence     = ptr[1];
                trace_instance     = r;
                /*
                    if (debug)
                    {
                        printf ("%d: ", trace_sentence & 0xFFFFFF);
                        if (dcb.sourcecount[trace_sentence >> 24]) {
                            printf (" %s", dcb.sourcelines[trace_sentence >> 24] [(trace_sentence & 0xFFFFFF)-1]) ;
                        }
                        printf ("\n");
                        fflush(stdout) ;
                    }
                */
                ptr += 2 ;
                break ;

            default:
                fprintf( stderr, "ERROR: Runtime error in %s(%d) - Mnemonic 0x%02X not implemented\n", r->proc->name, LOCDWORD( r, PROCESS_ID ), *ptr ) ;
                exit( 0 );
        }

        if ( r->stack_ptr < r->stack )
        {
            fprintf( stderr, "ERROR: Runtime error in %s(%d) - Critical Stack Problem StackBase=%p StackPTR=%p\n", r->proc->name, LOCDWORD( r, PROCESS_ID ), r->stack, r->stack_ptr ) ;
            exit( 0 );
        }

#ifdef EXIT_ON_EMPTY_STACK
        if ( r->stack_ptr == r->stack )
        {
            r->codeptr = ptr ;
            if ( LOCDWORD( r, STATUS ) != STATUS_RUNNING && LOCDWORD( r, STATUS ) != STATUS_DEAD ) break ;
        }
#endif

    }

    /* *** SALIDA GENERAL *** */
break_all:

    if ( !*ptr || *ptr == MN_RETURN || *ptr == MN_END || LOCDWORD( r, STATUS ) == STATUS_KILLED )
    {
        /* Check for waiting parent */
        if ( r->called_by && instance_exists( r->called_by ) && ( LOCDWORD( r->called_by, STATUS ) & STATUS_WAITING_MASK ) )
        {
            /* We're returning and the parent is waiting: wake it up */
            if ( r->called_by->stack && ( r->called_by->stack[0] & STACK_RETURN_VALUE ) )
                r->called_by->stack_ptr[-1] = return_value;

            LOCDWORD( r->called_by, STATUS ) &= ~STATUS_WAITING_MASK;
        }
        r->called_by = NULL;

        /* The process should be destroyed immediately, it is a function-type one */
        /* Ejecuto ONEXIT */
        if (( LOCDWORD( r, STATUS ) & ~STATUS_WAITING_MASK ) != STATUS_DEAD && r->exitcode )
        {
            LOCDWORD( r, STATUS ) = STATUS_DEAD;
            r->codeptr = r->code + r->exitcode;
            instance_go( r );
            if ( !instance_exists( r ) ) r = NULL;
        }
        else
        {
            instance_destroy( r );
            r = NULL ;
        }

        /* Hook from */
//        if (was_visible) object_list_dirty = 1;
        /* Hook to */
    }

    /* Hook */
    if ( r && instance_pos_execute_hook_count )
    {
        for ( n = 0; n < instance_pos_execute_hook_count; n++ )
            instance_pos_execute_hook_list[n]( r );
    }
    /* Hook */
    if ( r && LOCDWORD( r, STATUS ) != STATUS_KILLED && r->first_run ) r->first_run = 0;

    if ( debug_next && trace_sentence != -1 )
    {
        force_debug = 1;
        debug_next = 0;
    }

    return return_value;
}

/* ---------------------------------------------------------------------- */
