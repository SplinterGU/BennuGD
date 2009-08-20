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

#include <stdlib.h>
#include <string.h>

#include "bgdrtm.h"

#include "bgddl.h"
#include "dlvaracc.h"

#include "libvideo.h"

#ifdef _WIN32
#include <initguid.h>
#include "ddraw.h"
#endif

/* --------------------------------------------------------------------------- */

GRAPH * icon = NULL ;

SDL_Surface * screen = NULL ;
SDL_Surface * scale_screen = NULL ;

char * apptitle = NULL ;

int scr_width = 320 ;
int scr_height = 240 ;

int scr_initialized = 0 ;

int enable_16bits = 0 ;
int enable_32bits = 0 ;
int enable_scale = 0 ;
int scale_resolution = 0 ;
int full_screen = 0 ;
int double_buffer = 0 ;
int hardware_scr = 0 ;
int grab_input = 0 ;
int frameless = 0 ;
int scale_mode = SCALE_NONE ;
int waitvsync = 0 ;

/* --------------------------------------------------------------------------- */

DLCONSTANT  __bgdexport( libvideo, constants_def )[] =
{
    { "M320X200",           TYPE_DWORD, 3200200             },
    { "M320X240",           TYPE_DWORD, 3200240             },
    { "M320X400",           TYPE_DWORD, 3200400             },
    { "M360X240",           TYPE_DWORD, 3600240             },
    { "M376X282",           TYPE_DWORD, 3760282             },
    { "M400X300",           TYPE_DWORD, 4000300             },
    { "M512X384",           TYPE_DWORD, 5120384             },
    { "M640X400",           TYPE_DWORD, 6400400             },
    { "M640X480",           TYPE_DWORD, 6400480             },
    { "M800X600",           TYPE_DWORD, 8000600             },
    { "M1024X768",          TYPE_DWORD, 10240768            },
    { "M1280X1024",         TYPE_DWORD, 12801024            },

    { "MODE_WINDOW",        TYPE_DWORD, MODE_WINDOW         },
    { "MODE_2XSCALE",       TYPE_DWORD, MODE_2XSCALE        },
    { "MODE_FULLSCREEN",    TYPE_DWORD, MODE_FULLSCREEN     },
    { "MODE_DOUBLEBUFFER",  TYPE_DWORD, MODE_DOUBLEBUFFER   },
    { "MODE_HARDWARE",      TYPE_DWORD, MODE_HARDWARE       },

    { "MODE_WAITVSYNC",     TYPE_DWORD, MODE_WAITVSYNC      },
    { "WAITVSYNC",          TYPE_DWORD, MODE_WAITVSYNC      },

    { "DOUBLE_BUFFER",      TYPE_DWORD, MODE_DOUBLEBUFFER   },  /* Obsolete */
    { "HW_SURFACE",         TYPE_DWORD, MODE_HARDWARE       },  /* Obsolete */

    { "MODE_8BITS",         TYPE_DWORD, 8                   },
    { "MODE_16BITS",        TYPE_DWORD, 16                  },
    { "MODE_32BITS",        TYPE_DWORD, 32                  },

    { "MODE_8BPP",          TYPE_DWORD, 8                   },
    { "MODE_16BPP",         TYPE_DWORD, 16                  },
    { "MODE_32BPP",         TYPE_DWORD, 32                  },

    { "MODE_MODAL",         TYPE_DWORD, MODE_MODAL          },  /* GRAB INPU */
    { "MODE_FRAMELESS",     TYPE_DWORD, MODE_FRAMELESS      },  /* FRAMELESS window */

    { "SCALE_NONE",         TYPE_DWORD, SCALE_NONE          },

    { NULL          , 0         ,  0  }
} ;

/* --------------------------------------------------------------------------- */

#define GRAPH_MODE          0
#define SCALE_MODE          1
#define FULL_SCREEN         2
#define SCALE_RESOLUTION    3

/* --------------------------------------------------------------------------- */
/* Definicion de variables globales (usada en tiempo de compilacion) */

char * __bgdexport( libvideo, globals_def ) =
    "graph_mode = 0;\n"
    "scale_mode = 0;\n"
    "full_screen = 0;\n"
    "scale_resolution = 0;\n"
    ;

/* --------------------------------------------------------------------------- */
/* Son las variables que se desea acceder.                           */
/* El interprete completa esta estructura, si la variable existe.    */
/* (usada en tiempo de ejecucion)                                    */

DLVARFIXUP __bgdexport( libvideo, globals_fixup )[] =
{
    /* Nombre de variable global, puntero al dato, tamaño del elemento, cantidad de elementos */
    { "graph_mode" , NULL, -1, -1 },
    { "scale_mode" , NULL, -1, -1 },
    { "full_screen" , NULL, -1, -1 },
    { "scale_resolution", NULL, -1, -1 },
    { NULL , NULL, -1, -1 }
};

/* --------------------------------------------------------------------------- */

#ifdef _WIN32
/* Based allegro */

LPDIRECTDRAW2 directdraw = NULL;
DDCAPS ddcaps;

HRESULT WINAPI( *_DirectDrawCreate )( GUID FAR *lpGUID, LPDIRECTDRAW FAR *lplpDD, IUnknown FAR *pUnkOuter );

/* --------------------------------------------------------------------------- */

int init_dx( void )
{
    HINSTANCE handle;
    LPDIRECTDRAW directdraw1;
    HRESULT hr;
    LPVOID temp;

    handle = LoadLibrary( "DDRAW.DLL" );
    if ( handle == NULL ) return -1;

    _DirectDrawCreate = GetProcAddress( handle, "DirectDrawCreate" );

    hr = _DirectDrawCreate( NULL, &directdraw1, NULL );
    if ( FAILED( hr ) ) return -1;

    hr = IDirectDraw_QueryInterface( directdraw1, &IID_IDirectDraw2, &directdraw );
    if ( FAILED( hr ) ) return -1;

    IDirectDraw_Release( directdraw1 );

    hr = IDirectDraw2_SetCooperativeLevel( directdraw, NULL, DDSCL_NORMAL );
    if ( FAILED( hr ) ) return -1;

    /* get capabilities */
    ddcaps.dwSize = sizeof( ddcaps );
    hr = IDirectDraw2_GetCaps( directdraw, &ddcaps, NULL );
    if ( FAILED( hr ) ) return -1;

    return 0;
}
#endif

/* --------------------------------------------------------------------------- */

void gr_wait_vsync()
{
#ifdef _WIN32
    if ( directdraw ) IDirectDraw2_WaitForVerticalBlank( directdraw, DDWAITVB_BLOCKBEGIN, NULL );
#endif
}

/* --------------------------------------------------------------------------- */

void gr_set_caption( char * title )
{
    SDL_WM_SetCaption( apptitle = title, "" ) ;
}

/* --------------------------------------------------------------------------- */

int gr_set_icon( GRAPH * map )
{
    if (( icon = map ))
    {
        SDL_Surface *ico = NULL;
        if ( icon->format->depth == 8 )
        {
            SDL_Color palette[256];
            if ( sys_pixel_format && sys_pixel_format->palette )
            {
                int n ;
                for ( n = 0 ; n < 256 ; n++ )
                {
                    palette[ n ].r = sys_pixel_format->palette->rgb[ n ].r;
                    palette[ n ].g = sys_pixel_format->palette->rgb[ n ].g;
                    palette[ n ].b = sys_pixel_format->palette->rgb[ n ].b;
                }
            }

            ico = SDL_CreateRGBSurfaceFrom( icon->data, 32, 32, 8, 32, 0x00, 0x00, 0x00, 0x00 ) ;
            SDL_SetPalette( ico, SDL_LOGPAL, palette, 0, 256 );
        }
        else
        {
            ico = SDL_CreateRGBSurfaceFrom( icon->data, 32, 32, icon->format->depth, icon->pitch, icon->format->Rmask, icon->format->Gmask, icon->format->Bmask, icon->format->Amask ) ;
        }

        SDL_SetColorKey( ico, SDL_SRCCOLORKEY, SDL_MapRGB( ico->format, 0, 0, 0 ) ) ;
        SDL_WM_SetIcon( ico, NULL );
        SDL_FreeSurface( ico ) ;
    }

    return 1 ;
}

/* --------------------------------------------------------------------------- */

int gr_set_mode( int width, int height, int depth )
{
    int n ;
    int sdl_flags = 0;
    int surface_width = width;
    int surface_height = height;

    enable_scale = ( GLODWORD( libvideo, GRAPH_MODE ) & MODE_2XSCALE ) ? 1 : 0 ;
    full_screen = ( GLODWORD( libvideo, GRAPH_MODE ) & MODE_FULLSCREEN ) ? 1 : 0 ;
    double_buffer = ( GLODWORD( libvideo, GRAPH_MODE ) & MODE_DOUBLEBUFFER ) ? 1 : 0 ;
    hardware_scr = ( GLODWORD( libvideo, GRAPH_MODE ) & MODE_HARDWARE ) ? 1 : 0 ;
    grab_input = ( GLODWORD( libvideo, GRAPH_MODE ) & MODE_MODAL ) ? 1 : 0 ;
    frameless = ( GLODWORD( libvideo, GRAPH_MODE ) & MODE_FRAMELESS ) ? 1 : 0 ;
    waitvsync = ( GLODWORD( libvideo, GRAPH_MODE ) & MODE_WAITVSYNC ) ? 1 : 0 ;
    scale_mode = GLODWORD( libvideo, SCALE_MODE );
    full_screen |= GLODWORD( libvideo, FULL_SCREEN );
    scale_resolution = GLODWORD( libvideo, SCALE_RESOLUTION );

    if ( !depth )
    {
        enable_32bits = ( GLODWORD( libvideo, GRAPH_MODE ) & MODE_32BITS ) ? 1 : 0 ;
        if ( !enable_32bits )
            enable_16bits = ( GLODWORD( libvideo, GRAPH_MODE ) & MODE_16BITS ) ? 1 : 0 ;
        else
            enable_16bits = 0;
        depth = enable_32bits ? 32 : ( enable_16bits ? 16 : 8 );
    }
    else if ( depth == 16 )
    {
        enable_16bits = 1;
        enable_32bits = 0;
    }
    else if ( depth == 32 )
    {
        enable_16bits = 0;
        enable_32bits = 1;
    }

    if ( scale_resolution != 0                  &&
         ( scale_resolution / 10000 ) < width    &&
         ( scale_resolution % 10000 ) < height
       )
    {
        surface_width  = scale_resolution / 10000 ;
        surface_height = scale_resolution % 10000 ;
    }
    else
    {
        scale_resolution = 0;

        if ( scale_mode != SCALE_NONE ) enable_scale = 1;
        if ( enable_scale && scale_mode == SCALE_NONE ) scale_mode = SCALE_SCALE2X;

        if ( enable_scale )
        {
            enable_16bits = 1;
            depth = 16;

            surface_width  *= 2;
            surface_height *= 2;
        }
    }

    /* Inicializa el modo gráfico */

    if ( scrbitmap )
    {
        bitmap_destroy( scrbitmap ) ;
        scrbitmap = NULL ;
    }

    /* Setup the SDL Video Mode */

    sdl_flags = SDL_HWPALETTE;
    if ( double_buffer ) sdl_flags |= SDL_DOUBLEBUF;
    if ( full_screen ) sdl_flags |= SDL_FULLSCREEN;
    if ( frameless ) sdl_flags |= SDL_NOFRAME;

    sdl_flags |= hardware_scr ? SDL_HWSURFACE : SDL_SWSURFACE;

    if ( scale_screen ) SDL_FreeSurface( scale_screen ) ;
    if ( screen ) SDL_FreeSurface( screen ) ;

    if ( scale_resolution != 0 )
    {
        scale_screen = SDL_SetVideoMode( surface_width, surface_height, depth, sdl_flags );
        if ( !scale_screen ) return -1;
        screen = SDL_CreateRGBSurface( sdl_flags,
                                       width,
                                       height,
                                       scale_screen->format->BitsPerPixel,
                                       scale_screen->format->Rmask,
                                       scale_screen->format->Gmask,
                                       scale_screen->format->Bmask,
                                       scale_screen->format->Amask
                                     );
    }
    else
    {
        screen = SDL_SetVideoMode( surface_width, surface_height, depth, sdl_flags );
    }

    if ( !screen ) return -1;

    SDL_WM_GrabInput( grab_input ? SDL_GRAB_ON : SDL_GRAB_OFF ) ;

    /* Set window title */
    gr_set_caption( apptitle ) ;

    if ( !sys_pixel_format )
    {
        sys_pixel_format = bitmap_create_format( depth );
    }
    else
    {
        PALETTE * p = sys_pixel_format->palette;

        free( sys_pixel_format );
        sys_pixel_format = bitmap_create_format( depth );

        if ( p )
        {
            sys_pixel_format->palette = p;
            pal_refresh( sys_pixel_format->palette ) ;
        }
    }

    if ( sys_pixel_format->depth == 16 )
    {
        for ( n = 0 ; n < 65536 ; n++ )
        {
            colorghost[ n ] =
                ((( n & screen->format->Rmask ) >> 1 ) & screen->format->Rmask ) +
                ((( n & screen->format->Gmask ) >> 1 ) & screen->format->Gmask ) +
                ((( n & screen->format->Bmask ) >> 1 ) & screen->format->Bmask ) ;
        }
//        bitmap_16bits_conversion();
    }

    scr_initialized = 1 ;

    SDL_ShowCursor( 0 ) ;

    pal_refresh( NULL ) ;
    palette_changed = 1 ;

//    gr_make_trans_table();

    /* Bitmaps de fondo */

    /* Only allow background with same properties that video mode */
    if (
        !background ||
        scr_width != width || scr_height != height ||
        sys_pixel_format->depth != background->format->depth )
    {
        if ( background ) bitmap_destroy( background );
        background = bitmap_new( 0, width, height, sys_pixel_format->depth ) ;
        if ( background )
        {
            gr_clear( background ) ;
            bitmap_add_cpoint( background, 0, 0 ) ;
        }
    }

    scr_width = width ;
    scr_height = height ;

    regions[0].x  = 0 ;
    regions[0].y  = 0 ;
    regions[0].x2 = width - 1 ;
    regions[0].y2 = height - 1 ;

    // Finalmente seteamos icono de aplicacion
    // Necesitamos crear una surface a partir de un MAP generico de 16x16...
    gr_set_icon( icon );

    if ( background ) background->modified = 1;

//    gr_rects_clear();

    return 0;
}

/* --------------------------------------------------------------------------- */

int gr_init( int width, int height )
{
    return gr_set_mode( width, height, 0 );
}

/* --------------------------------------------------------------------------- */

void __bgdexport( libvideo, module_initialize )()
{
    char * e;

    if ( !SDL_WasInit( SDL_INIT_VIDEO ) ) SDL_InitSubSystem( SDL_INIT_VIDEO );

#ifdef _WIN32
    if ( !directdraw ) init_dx();
#endif
    apptitle = appname;

    e = getenv("VIDEO_WIDTH");
    if ( e ) scr_width = atoi(e);

    e = getenv("VIDEO_HEIGHT");
    if ( e ) scr_height = atoi(e);

    e = getenv("VIDEO_DEPTH");
    if ( e )
        GLODWORD( libvideo, GRAPH_MODE ) = atoi(e);
    else
#ifdef TARGET_GP2X_WIZ
        GLODWORD( libvideo, GRAPH_MODE ) = MODE_16BITS;
#else
        GLODWORD( libvideo, GRAPH_MODE ) = MODE_32BITS;
#endif

    gr_init( scr_width, scr_height ) ;
}

/* --------------------------------------------------------------------------- */

void __bgdexport( libvideo, module_finalize )()
{
#ifdef _WIN32
    if ( directdraw )
    {
        /* set cooperative level back to normal */
        IDirectDraw2_SetCooperativeLevel( directdraw, NULL, DDSCL_NORMAL );

        /* release DirectDraw interface */
        IDirectDraw2_Release( directdraw );

        directdraw = NULL;
    }
#endif
    if ( SDL_WasInit( SDL_INIT_VIDEO ) ) SDL_QuitSubSystem( SDL_INIT_VIDEO );
}

/* --------------------------------------------------------------------------- */

char * __bgdexport( libvideo, modules_dependency )[] =
{
    "libgrbase",
    NULL
};

/* --------------------------------------------------------------------------- */
