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

/* --------------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>

#include "bgddl.h"
#include "dlvaracc.h"

#include "libgrbase.h"
#include "libblit.h"
#include "librender.h"
#include "libvideo.h"
#include "resolution.h"

#include "libscroll.h"

/* --------------------------------------------------------------------------- */

/* Indicadores de bloqueo */
#define GRAPH_HWRAP 1
#define GRAPH_VWRAP 2
#define BACK_HWRAP 4
#define BACK_VWRAP 8

/* --------------------------------------------------------------------------- */

int scrolls_objects[ 10 ] = { 0 };

scrolldata scrolls[ 10 ] ;

/* --------------------------------------------------------------------------- */

/* Locals */

#define CTYPE           0
#define CNUMBER         1
#define PROCESS_ID      2
#define STATUS          3
#define COORDX          4
#define COORDY          5
#define COORDZ          6
#define RESOLUTION      7

/* Globals */

#define SCROLLS         0

/* --------------------------------------------------------------------------- */

DLCONSTANT __bgdexport( libscroll, constants_def )[] =
{
    { "C_SCROLL",   TYPE_DWORD,     C_SCROLL    },

    { "C_0",        TYPE_DWORD,     0x0001      },
    { "C_1",        TYPE_DWORD,     0x0002      },
    { "C_2",        TYPE_DWORD,     0x0004      },
    { "C_3",        TYPE_DWORD,     0x0008      },
    { "C_4",        TYPE_DWORD,     0x0010      },
    { "C_5",        TYPE_DWORD,     0x0020      },
    { "C_6",        TYPE_DWORD,     0x0040      },
    { "C_7",        TYPE_DWORD,     0x0080      },
    { "C_8",        TYPE_DWORD,     0x0100      },
    { "C_9",        TYPE_DWORD,     0x0200      },

    { NULL,         0,              0           }
};

/* --------------------------------------------------------------------------- */

char * __bgdexport( libscroll, locals_def ) =
    "ctype;\n"
    "cnumber;\n";

/* --------------------------------------------------------------------------- */

char * __bgdexport( libscroll, globals_def ) =
    "STRUCT scroll[9]\n"
    "x0, y0;\n"
    "x1, y1;\n"
    "z = 512;\n"
    "camera;\n"
    "ratio = 200;\n"
    "speed;\n"
    "region1 = -1;\n"
    "region2 = -1;\n"
    "flags1;\n"
    "flags2;\n"
    "follow = -1;\n"
    "reserved[6];\n"  /* size: 20 dwords */
    "END \n";

/* --------------------------------------------------------------------------- */

DLVARFIXUP __bgdexport( libscroll, locals_fixup )[] =
{
    /* Nombre de variable global, puntero al dato, tamaño del elemento, cantidad de elementos */
    { "ctype" , NULL, -1, -1 },
    { "cnumber" , NULL, -1, -1 },
    { "id", NULL, -1, -1 },
    { "reserved.status", NULL, -1, -1 },
    { "x" , NULL, -1, -1 },
    { "y" , NULL, -1, -1 },
    { "z" , NULL, -1, -1 },
    { "resolution" , NULL, -1, -1 },
    { NULL , NULL, -1, -1 }
};

/* --------------------------------------------------------------------------- */

DLVARFIXUP __bgdexport( libscroll, globals_fixup )[] =
{
    /* Nombre de variable global, puntero al dato, tamaño del elemento, cantidad de elementos */
    { "scroll" , NULL, -1, -1 },
    { NULL , NULL, -1, -1 }
};

/* --------------------------------------------------------------------------- */

static void draw_scroll( int n, REGION * clip );
static int info_scroll( int n, REGION * clip, int * z, int * drawme );

/* --------------------------------------------------------------------------- */

void scroll_region( int n, REGION * r )
{
    if ( n < 0 || n > 9 ) return ;

    /* Corrected from x,y... to posx,posy... so out_region works fine */
    r->x  -= scrolls[n].posx0 ;
    r->y  -= scrolls[n].posy0 ;
    r->x2 -= scrolls[n].posx0 ;
    r->y2 -= scrolls[n].posy0 ;
}

/* --------------------------------------------------------------------------- */

void scroll_start( int n, int fileid, int graphid, int backid, int region, int flags )
{
    SCROLL_EXTRA_DATA * data;

    if ( n >= 0 && n <= 9 )
    {
        if ( region < 0 || region > 31 ) region = 0 ;

        scrolls[n].graph = graphid ? bitmap_get( fileid, graphid ) : 0 ;
        scrolls[n].back  = backid  ? bitmap_get( fileid, backid )  : 0 ;

        if ( !graphid || !scrolls[n].graph ) return ; // El fondo de scroll no existe
        if ( backid && !scrolls[n].back ) return ; // Grafico no existe

        scrolls[n].active  = 1 ;
        scrolls[n].fileid  = fileid ;
        scrolls[n].graphid = graphid ;
        scrolls[n].backid  = backid ;
        scrolls[n].region  = &regions[region] ;
        scrolls[n].flags   = flags ;

        data = &(( SCROLL_EXTRA_DATA * ) & GLODWORD( libscroll, SCROLLS ) )[n] ;

        data->reserved[0] = ( int32_t ) & scrolls[n]; /* First reserved dword point to internal scrolldata struct */

        if ( scrolls_objects[n] ) gr_destroy_object( scrolls_objects[n] );
        scrolls_objects[n] = ( int )gr_new_object( 0, info_scroll, draw_scroll, n );
    }
}

/* --------------------------------------------------------------------------- */

void scroll_stop( int n )
{
    if ( n >= 0 && n <= 9 )
    {
        if ( scrolls_objects[n] )
        {
            gr_destroy_object( scrolls_objects[n] );
            scrolls_objects[n] = 0;
            scrolls[n].active = 0 ;
        }
    }
}

/* --------------------------------------------------------------------------- */

void scroll_update( int n )
{
    int x0, y0, x1, y1, cx, cy, w, h, speed ;

    REGION bbox;
    GRAPH * gr;

    SCROLL_EXTRA_DATA * data;

    if ( n < 0 || n > 9 ) return ;

    data = &(( SCROLL_EXTRA_DATA * ) & GLODWORD( libscroll, SCROLLS ) )[n] ;

    if ( !scrolls[n].active || !scrolls[n].region || !scrolls[n].graph ) return ;

    w = scrolls[n].region->x2 - scrolls[n].region->x + 1 ;
    h = scrolls[n].region->y2 - scrolls[n].region->y + 1 ;

    scrolls[n].z       = data->z ;
    scrolls[n].ratio   = data->ratio ;
    scrolls[n].camera  = instance_get( data->camera ) ;
    scrolls[n].speed   = data->speed ;

    if ( data->follows < 0 || data->follows > 9 )
        scrolls[n].follows = 0 ;
    else
        scrolls[n].follows = &scrolls[data->follows] ;

    if ( data->region1 < 0  || data->region1 > 31 )
        scrolls[n].region1 = 0 ;
    else
        scrolls[n].region1 = &regions[data->region1] ;

    if ( data->region2 < 0  || data->region2 > 31 )
        scrolls[n].region2 = 0 ;
    else
        scrolls[n].region2 = &regions[data->region2] ;

    /* Actualiza las variables globales (perseguir la camara, etc) */

    if ( scrolls[n].follows )
    {
        if ( scrolls[n].ratio )
        {
            data->x0 = scrolls[n].follows->x0 * 100 / scrolls[n].ratio ;
            data->y0 = scrolls[n].follows->y0 * 100 / scrolls[n].ratio ;
        }
        else
        {
            data->x0 = scrolls[n].follows->x0 ;
            data->y0 = scrolls[n].follows->y0 ;
        }
    }

    if ( scrolls[n].camera )
    {
        /* Mira a ver si entra dentro de la region 1 o 2 */

        speed = scrolls[n].speed ;
        if ( scrolls[n].speed == 0 ) speed = 9999999 ;

        /* Update speed */

        if (( gr = instance_graph( scrolls[n].camera ) ) )
        {
            instance_get_bbox( scrolls[n].camera, gr, &bbox );

            x0 = bbox.x - data->x0 ;
            y0 = bbox.y - data->y0 ;
            x1 = bbox.x2 - data->x0 ;
            y1 = bbox.y2 - data->y0 ;

            if ( scrolls[n].region1 &&
                    (
                        x0 < scrolls[n].region1->x2 && y0 < scrolls[n].region1->y2 &&
                        x1 > scrolls[n].region1->x  && y1 > scrolls[n].region1->y
                    )
               )
            {
                speed = 0 ;
            }
            else
                if ( scrolls[n].region2 )
                {
                    if ( x0 > scrolls[n].region2->x2 ) speed = ( x0 - scrolls[n].region2->x2 );
                    if ( y0 > scrolls[n].region2->y2 ) speed = ( y0 - scrolls[n].region2->y2 );
                    if ( x1 < scrolls[n].region2->x ) speed = ( scrolls[n].region2->x - x1 );
                    if ( y1 < scrolls[n].region2->y ) speed = ( scrolls[n].region2->y - y1 );
                }
        }

        /* Forzar a que esté en el centro de la ventana */

        cx = LOCDWORD( libscroll, scrolls[n].camera, COORDX ) ;
        cy = LOCDWORD( libscroll, scrolls[n].camera, COORDY ) ;

        RESOLXY( libscroll, scrolls[n].camera, cx, cy );

        cx -= w / 2 ;
        cy -= h / 2 ;

        if ( data->x0 < cx ) data->x0 = MIN( data->x0 + speed, cx ) ;
        if ( data->y0 < cy ) data->y0 = MIN( data->y0 + speed, cy ) ;
        if ( data->x0 > cx ) data->x0 = MAX( data->x0 - speed, cx ) ;
        if ( data->y0 > cy ) data->y0 = MAX( data->y0 - speed, cy ) ;
    }

    /* Scrolls no cíclicos y posición del background */

    if ( scrolls[n].graph )
    {
        if ( !( scrolls[n].flags & GRAPH_HWRAP ) ) data->x0 = MAX( 0, MIN( data->x0, ( int )scrolls[n].graph->width  - w ) ) ;
        if ( !( scrolls[n].flags & GRAPH_VWRAP ) ) data->y0 = MAX( 0, MIN( data->y0, ( int )scrolls[n].graph->height - h ) ) ;
    }

    if ( scrolls[n].ratio )
    {
        data->x1 = data->x0 * 100 / scrolls[n].ratio ;
        data->y1 = data->y0 * 100 / scrolls[n].ratio ;
    }

    if ( scrolls[n].back )
    {
        if ( !( scrolls[n].flags & BACK_HWRAP ) ) data->x1 = MAX( 0, MIN( data->x1, ( int )scrolls[n].back->width  - w ) ) ;
        if ( !( scrolls[n].flags & BACK_VWRAP ) ) data->y1 = MAX( 0, MIN( data->y1, ( int )scrolls[n].back->height - h ) ) ;
    }

    /* Actualiza la posición del scroll según las variables globales */

    scrolls[n].posx0 = data->x0 ;
    scrolls[n].posy0 = data->y0 ;
    scrolls[n].x0 = data->x0 % ( int32_t ) scrolls[n].graph->width ;
    scrolls[n].y0 = data->y0 % ( int32_t ) scrolls[n].graph->height ;

    if ( scrolls[n].x0 < 0 ) scrolls[n].x0 += scrolls[n].graph->width ;
    if ( scrolls[n].y0 < 0 ) scrolls[n].y0 += scrolls[n].graph->height ;

    if ( scrolls[n].back )
    {
        scrolls[n].x1 = data->x1 % ( int32_t ) scrolls[n].back->width ;
        scrolls[n].y1 = data->y1 % ( int32_t ) scrolls[n].back->height ;
        if ( scrolls[n].x1 < 0 ) scrolls[n].x1 += scrolls[n].back->width ;
        if ( scrolls[n].y1 < 0 ) scrolls[n].y1 += scrolls[n].back->height ;
    }
}

/* --------------------------------------------------------------------------- */

static int compare_instances( const void * ptr1, const void * ptr2 )
{
    const INSTANCE * i1 = *( const INSTANCE ** )ptr1 ;
    const INSTANCE * i2 = *( const INSTANCE ** )ptr2 ;

    int ret = LOCDWORD( libscroll, i2, COORDZ ) - LOCDWORD( libscroll, i1, COORDZ );

    return !ret ? LOCDWORD( libscroll, i1, PROCESS_ID ) - LOCDWORD( libscroll, i2, PROCESS_ID ) : ret;
}

/* --------------------------------------------------------------------------- */

void scroll_draw( int n, REGION * clipping )
{
    int nproc, x, y, cx, cy ;

    static INSTANCE ** proclist = 0;
    static int proclist_reserved = 0;
    int proclist_count;
    REGION r;
    int status;

    SCROLL_EXTRA_DATA * data;
    INSTANCE * i;

    if ( n < 0 || n > 9 ) return ;

    data = &(( SCROLL_EXTRA_DATA * ) & GLODWORD( libscroll, SCROLLS ) )[n] ;

    if ( !scrolls[n].active || !scrolls[n].region || !scrolls[n].graph ) return ;

    /* Dibuja el fondo */

    r = *scrolls[n].region;
    if ( clipping ) region_union( &r, clipping );

    if ( scrolls[n].back )
    {
        if ( scrolls[n].back->ncpoints > 0 && scrolls[n].back->cpoints[0].x >= 0 )
        {
            cx = scrolls[n].back->cpoints[0].x ;
            cy = scrolls[n].back->cpoints[0].y ;
        }
        else
        {
            cx = scrolls[n].back->width / 2 ;
            cy = scrolls[n].back->height / 2 ;
        }

        y = scrolls[n].region->y - scrolls[n].y1 ;

        while ( y < scrolls[n].region->y2 )
        {
            x = scrolls[n].region->x - scrolls[n].x1 ;
            while ( x < scrolls[n].region->x2 )
            {
                gr_blit( 0, &r, x + cx, y + cy, data->flags2, scrolls[n].back ) ;
                x += scrolls[n].back->width ;
            }
            y += scrolls[n].back->height ;
        }
    }

    /* Dibuja el primer plano */

    if ( scrolls[n].graph->ncpoints > 0 && scrolls[n].graph->cpoints[0].x >= 0 )
    {
        cx = scrolls[n].graph->cpoints[0].x ;
        cy = scrolls[n].graph->cpoints[0].y ;
    }
    else
    {
        cx = scrolls[n].graph->width / 2 ;
        cy = scrolls[n].graph->height / 2 ;
    }

    y = scrolls[n].region->y - scrolls[n].y0 ;
    while ( y < scrolls[n].region->y2 )
    {
        x = scrolls[n].region->x - scrolls[n].x0 ;
        while ( x < scrolls[n].region->x2 )
        {
            gr_blit( 0, &r, x + cx, y + cy, data->flags1, scrolls[n].graph ) ;
            x += scrolls[n].graph->width ;
        }
        y += scrolls[n].graph->height ;
    }

    /* Crea una lista ordenada de instancias a dibujar */

    i = first_instance ;
    proclist_count = 0 ;
    while ( i )
    {
        if ( LOCDWORD( libscroll, i, CTYPE ) == C_SCROLL &&
                (
                    (( status = LOCDWORD( libscroll, i, STATUS ) ) & ~STATUS_WAITING_MASK ) == STATUS_RUNNING ||
                    ( status & ~STATUS_WAITING_MASK ) == STATUS_FROZEN
                )
           )
        {
            if ( LOCDWORD( libscroll, i, CNUMBER ) && !( LOCDWORD( libscroll, i, CNUMBER ) & ( 1 << n ) ) )
            {
                i = i->next ;
                continue ;
            }

            if ( proclist_count == proclist_reserved )
            {
                proclist_reserved += 16 ;
                proclist = ( INSTANCE ** ) realloc( proclist, sizeof( INSTANCE * ) * proclist_reserved ) ;
            }

            proclist[proclist_count++] = i ;
        }
        i = i->next ;
    }

    if ( proclist_count )
    {
        /* Ordena la listilla */

        qsort( proclist, proclist_count, sizeof( INSTANCE * ), compare_instances ) ;

        /* Visualiza los procesos */

        for ( nproc = 0 ; nproc < proclist_count ; nproc++ )
        {
            x = LOCDWORD( libscroll, proclist[nproc], COORDX ) ;
            y = LOCDWORD( libscroll, proclist[nproc], COORDY ) ;

            RESOLXY( libscroll, proclist[nproc], x, y );

            draw_instance_at( proclist[nproc], &r, x - scrolls[n].posx0 + scrolls[n].region->x, y - scrolls[n].posy0 + scrolls[n].region->y ) ;
        }
    }
}

/* --------------------------------------------------------------------------- */

static void draw_scroll( int n, REGION * clip )
{
    scroll_draw( n, clip ) ;
}

/* --------------------------------------------------------------------------- */

static int info_scroll( int n, REGION * clip, int * z, int * drawme )
{
    * z = scrolls[n].z;
    * drawme = 1;
    * clip = * scrolls[n].region;
    scroll_update( n );
    return 1;
}

/* --------------------------------------------------------------------------- */

char * __bgdexport( libscroll, modules_dependency )[] =
{
    "libgrbase",
    "libblit",
    "librender",
    "libvideo",
    NULL
};

/* --------------------------------------------------------------------------- */
