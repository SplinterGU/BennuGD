/*
 *  Copyright � 2006-2008 SplinterGU (Fenix/Bennugd)
 *  Copyright � 2002-2006 Fenix Team (Fenix)
 *  Copyright � 1999-2002 Jos� Luis Cebri�n Pag�e (Fenix)
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

static GRAPH * scrbitmap_extra = NULL ;

static int updaterects_count = 0;
static REGION updaterects[128];
static SDL_Rect rects[128];

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : gr_draw_screen
 *
 *  Draw the current screen, using the given restore_type / dump_type parameters
 *
 *  PARAMS :
 *      dest            Destination graphic (MUST have a correct size)
 *
 *      restore_type
 *          -1          No background restore
 *           0          Partial (old process bounding box) background restore
 *           1          Full background restore
 *
 *      dump_type
 *           0          Partial (new process bounding box) drawing
 *           1          Full drawing
 *
 *  RETURN VALUE :
 *      None
 */

void gr_draw_screen( GRAPH * dest, int restore_type, int dump_type )
{
    int a ;
    GRAPH * orig_scrbitmap = scrbitmap;

    scrbitmap = dest ;

    if ( background && background->modified )
    {
        restore_type = 1;
        dump_type = 1;
    }

    /* Update the object list */

    gr_update_objects_mark_rects( restore_type, dump_type );

    /* Restore the background */

    if ( dump_type == 0 || restore_type == 0 )
    {
        updaterects_count = gr_mark_rects( updaterects );
    }
    else
    {
        updaterects_count = 1;
        updaterects[ 0 ].x = 0;
        updaterects[ 0 ].y = 0;
        updaterects[ 0 ].x2 = scr_width - 1;
        updaterects[ 0 ].y2 = scr_height - 1;
    }

    if ( !restore_type )
    {
        /* PARTIAL_RESTORE */
        if ( background->info_flags & GI_CLEAN )
        {
            for ( a = 0; a < updaterects_count; a++ )
                gr_clear_region( scrbitmap, &updaterects[ a ] );
        }
        else
        {
            for ( a = 0; a < updaterects_count; a++ )
                gr_blit( scrbitmap, &updaterects[ a ], 0, 0, B_NOCOLORKEY, background );
        }
    }
    else if ( restore_type == 1 )
    {
        /* COMPLETE_RESTORE */

        if ( background->info_flags & GI_CLEAN )
            gr_clear( scrbitmap ) ;
        else
            gr_blit( scrbitmap, NULL, 0, 0, B_NOCOLORKEY, background );

        updaterects_count = 1;
        updaterects[ 0 ].x = 0;
        updaterects[ 0 ].y = 0;
        updaterects[ 0 ].x2 = scr_width - 1;
        updaterects[ 0 ].y2 = scr_height - 1;
    }

    /* Dump the objects */

    if ( dump_type == 0 )
    {
        /* Dump only changed & enabled objects */
        gr_draw_objects( updaterects, updaterects_count );
    }
    else
    {
        /* Dump everything */
        gr_draw_objects_complete();
    }

    /* Reset the zone-to-update array for the next frame */
    gr_rects_clear();

    if ( background && background->modified ) background->modified = 0;
    if ( scrbitmap && scrbitmap->modified ) scrbitmap->modified = 0;

    scrbitmap = orig_scrbitmap;
}

/* --------------------------------------------------------------------------- */

static int screen_locked = 0 ;

int gr_lock_screen()
{
    if ( screen_locked ) return 1 ;

    screen_locked = 1 ;

    if ( SDL_MUSTLOCK( screen ) ) SDL_LockSurface( screen ) ;

    if ( enable_scale || scale_mode != SCALE_NONE )
    {
        if ( scrbitmap && ( scrbitmap->info_flags & GI_EXTERNAL_DATA ) )
        {
            bitmap_destroy( scrbitmap ) ;
            scrbitmap = NULL ;
        }

        if ( !scrbitmap )
        {
            scrbitmap = bitmap_new( 0, screen->w / 2, screen->h / 2, sys_pixel_format->depth ) ;
            bitmap_add_cpoint( scrbitmap, 0, 0 ) ;
        }
    }
    else
    {
        if ( !scrbitmap || !( scrbitmap->info_flags & GI_EXTERNAL_DATA ) )
        {
            if ( scrbitmap ) bitmap_destroy( scrbitmap ) ;
            scrbitmap = bitmap_new_ex( 0, screen->w, screen->h, screen->format->BitsPerPixel, screen->pixels, screen->pitch );
            bitmap_add_cpoint( scrbitmap, 0, 0 ) ;
        }
    }

    return 1 ;
}

/* --------------------------------------------------------------------------- */

void gr_unlock_screen()
{
    if ( !screen_locked || !screen->pixels ) return ;

    screen_locked = 0 ;

    if ( enable_scale )
    {
        GRAPH * scr;

        if ( scrbitmap->format->depth == 8 )
        {
            uint8_t * original, * poriginal;
            uint16_t * extra, * pextra;
            int n = scrbitmap->height, length;

            if (
                !scrbitmap_extra ||
                scrbitmap_extra->width != scrbitmap->width ||
                scrbitmap_extra->height != scrbitmap->height
            )
            {
                if ( scrbitmap_extra ) bitmap_destroy( scrbitmap_extra );
                scrbitmap_extra = bitmap_new( 0, scrbitmap->width, scrbitmap->height, 16 );
            }

            poriginal = scrbitmap->data;
            pextra = scrbitmap_extra->data;

            while ( n-- )
            {
                original = poriginal;
                extra = pextra;
                length = scrbitmap->width;
                while ( length-- ) *extra++ = sys_pixel_format->palette->colorequiv[ *original++ ];
                poriginal += scrbitmap->pitch;
                pextra = ( uint16_t * )((( uint8_t * ) pextra ) + scrbitmap_extra->pitch );
            }

            scr = scrbitmap_extra;
        }
        else
        {
            scr = scrbitmap;
        }

        /* Esto podria ir en un modulo aparte */
        switch ( scale_mode )
        {
            case SCALE_SCALE2X:
                scale2x( scr->data, scr->pitch, screen->pixels, screen->pitch, scr->width, scr->height );
                break;

            case SCALE_HQ2X:
                hq2x( scr->data, scr->pitch, screen->pixels, screen->pitch, scr->width, scr->height );
                break;

            case SCALE_SCANLINE2X:
                scanline2x( scr->data, scr->pitch, screen->pixels, screen->pitch, scr->width, scr->height );
                break;

            case SCALE_NOFILTER:
                scale_normal2x( scr->data, scr->pitch, screen->pixels, screen->pitch, scr->width, scr->height );
                break;

            case SCALE_NONE:
                // No usado
                break;
        }

        if ( SDL_MUSTLOCK( screen ) ) SDL_UnlockSurface( screen ) ;
        if ( waitvsync ) gr_wait_vsync();
        SDL_Flip( screen ) ;
    }
    else if ( scrbitmap->info_flags & GI_EXTERNAL_DATA )
    {
        if ( double_buffer ||
                (
                    updaterects_count == 1 &&
                    rects->x == 0 &&
                    rects->y == 0 &&
                    rects->w == scr_width - 1 &&
                    rects->h == scr_height - 1
                )
           )
        {
            if ( SDL_MUSTLOCK( screen ) ) SDL_UnlockSurface( screen ) ;
            if ( waitvsync ) gr_wait_vsync();
            SDL_Flip( screen ) ;
        }
        else
        {
            if ( updaterects_count )
            {
                SDL_Rect rects[ 128 ];
                int i;

                for ( i = 0 ; i < updaterects_count ; i++ )
                {
                    rects[ i ].x = updaterects[ i ].x;
                    rects[ i ].y = updaterects[ i ].y;
                    rects[ i ].w = ( updaterects[ i ].x2 - rects[ i ].x + 1 );
                    rects[ i ].h = ( updaterects[ i ].y2 - rects[ i ].y + 1 );
                }
                if ( SDL_MUSTLOCK( screen ) ) SDL_UnlockSurface( screen ) ;
                if ( waitvsync ) gr_wait_vsync();
                SDL_UpdateRects( screen, updaterects_count, rects ) ;
            }
        }
    }
}

/* --------------------------------------------------------------------------- */