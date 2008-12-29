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
#include <time.h>

#define __LIB_RENDER
#include "librender.h"

/* --------------------------------------------------------------------------- */

uint32_t frame_count = 0 ;
int last_ticks_ok = 0 ;
int last_frame_ticks = 0 ;
int next_frame_ticks = 0 ;
int fps_value = 25 ;
int max_jump = 2 ;
float frame_ms = 40.0 ;
int jump = 0 ;
int FPS_count = 0 ;
int FPS_init = 0 ;

/* --------------------------------------------------------------------------- */
/* Inicialización y controles de tiempo                                        */
/* --------------------------------------------------------------------------- */

/*
 *  FUNCTION : gr_set_fps
 *
 *  Change the game fps and frameskip values
 *
 *  PARAMS :
 *      fps         New number of frames per second
 *      jump        New value of maximum frameskip
 *
 *  RETURN VALUE :
 *      None
 */

void gr_set_fps( int fps, int jump )
{
    frame_ms = fps ? 1000.0 / fps : 0.0 ;
    max_jump = jump ;
    fps_value = fps;

    next_frame_ticks = SDL_GetTicks() + frame_ms ;
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : gr_wait_frame
 *
 *  Wait for the next frame start.
 *
 *  PARAMS :
 *      None
 *
 *  RETURN VALUE :
 *      None
 */

void gr_wait_frame()
{
    int frame_ticks, delay ;

    /* Tomo Tick actual */
    frame_ticks = SDL_GetTicks() ;

    /* Tiempo transcurrido total del ejecucion del ultimo frame (Frame time en ms) */
    * ( float * ) & GLODWORD( librender, FRAME_TIME ) = ( frame_ticks - last_frame_ticks ) / 1000.0f;

    /* -------------- */

    FPS_count++ ;

    /* Si paso 1 segundo o mas desde la ultima lectura */
    if ( FPS_init + 1000 < frame_ticks )
    {
        if ( fps_value )
            GLODWORD( librender, SPEED_GAUGE ) = FPS_count * 100 / fps_value ;
        else
            GLODWORD( librender, SPEED_GAUGE ) = 100;

        GLODWORD( librender, FPS ) = FPS_count ;
        FPS_count = 0 ;
        FPS_init = frame_ticks ;
    }

    /* -------------- */

    frame_count++ ;

    #ifdef DEBUG_FRAMES
    printf ( "%s:%d-> tiempo_esperado de frame: %g / saltos: %d / tiempo consumido: %d / tiempo esperado: %g\n", __FILE__, __LINE__, frame_ms, jump, ( frame_ticks - last_ticks_ok ), frame_ms * (1 + jump) ) ; fflush ( stdout ) ;
    #endif

    /* Si supere el tiempo previsto de ejecucion del frame */
    if ( frame_ms && ( frame_ticks - last_ticks_ok ) > frame_ms * ( 1 + jump ) )
    {
        /* Como no me alcanza el tiempo, voy a hacer skip */
        if ( jump < max_jump )
            jump++ ; /* No dibujar el frame */
        else
            jump = 0 ;
    }
    else
    {
        /* Estoy dentro del tiempo esperado, ejecuto un delay, para no superar los fps */
        delay = next_frame_ticks - frame_ticks ;
        if ( delay > 0 ) SDL_Delay( delay ) ;
        jump = 0 ;

        /* Restauro frame_ticks */
        frame_ticks = SDL_GetTicks();
    }

    /* Calculo el proximo frame */
    if ( !jump )
    {
        last_ticks_ok = frame_ticks;
//        next_frame_ticks += frame_ms ;
        next_frame_ticks = frame_ticks + frame_ms ;
    }

    /* Tiempo inicial del nuevo frame */
    last_frame_ticks = frame_ticks;
}

/* --------------------------------------------------------------------------- */

static SDL_Color palette[256];

void gr_refresh_palette()
{
    int n ;

    if ( sys_pixel_format->depth > 8 )
    {
        if ( sys_pixel_format->palette )
            for ( n = 0 ; n < 256 ; n++ )
                sys_pixel_format->palette->colorequiv[ n ] = gr_map_rgb
                        (
                            sys_pixel_format,
                            sys_pixel_format->palette->rgb[ n ].r,
                            sys_pixel_format->palette->rgb[ n ].g,
                            sys_pixel_format->palette->rgb[ n ].b
                        ) ;
    }
    else if ( sys_pixel_format->depth == 8 )
    {
        if ( sys_pixel_format->palette )
        {
            for ( n = 0 ; n < 256 ; n++ )
            {
                palette[ n ].r = sys_pixel_format->palette->rgb[ n ].r;
                palette[ n ].g = sys_pixel_format->palette->rgb[ n ].g;
                palette[ n ].b = sys_pixel_format->palette->rgb[ n ].b;
            }
        }
        else
        {
            uint8_t * pal = default_palette;
            for ( n = 0 ; n < 256 ; n++ )
            {
                palette[ n ].r = *pal++;
                palette[ n ].g = *pal++;
                palette[ n ].b = *pal++;
            }
        }
        SDL_SetColors( screen, palette, 0, 256 ) ;
    }

    palette_changed = 0;
    trans_table_updated = 0 ;
}

/* --------------------------------------------------------------------------- */

void gr_draw_frame()
{
    if ( jump ) return ;

    /* Actualiza paleta */

    if ( palette_changed ) gr_refresh_palette();

    if ( !trans_table_updated ) gr_make_trans_table();

    /* Bloquea el bitmap de pantalla */

    if ( gr_lock_screen() < 0 ) return ;

    /* Dibuja la pantalla */

    gr_draw_screen( scrbitmap, GLODWORD( librender, RESTORETYPE ), GLODWORD( librender, DUMPTYPE ) );

    /* Fading */

    if ( fade_on || fade_set )
    {
        gr_fade_step() ;
        if ( background ) background->modified = 1 ;
    }

    /* Actualiza la paleta y la pantalla */

    gr_unlock_screen() ;

}

/* --------------------------------------------------------------------------- */

void __bgdexport( librender, module_initialize )()
{
    if ( !SDL_WasInit( SDL_INIT_TIMER ) ) SDL_InitSubSystem( SDL_INIT_TIMER );
}

/* --------------------------------------------------------------------------- */

void __bgdexport( librender, module_finalize )()
{
    if ( SDL_WasInit( SDL_INIT_TIMER ) ) SDL_QuitSubSystem( SDL_INIT_TIMER );
}

/* --------------------------------------------------------------------------- */
