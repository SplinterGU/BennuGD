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

#define _LIBKEY_C

#include <stdio.h>
#include <stdlib.h>

#ifdef TARGET_MAC
#include <SDL/SDL.h>
#else
#include <SDL.h>
#endif

#include "xctype.h"
#include "bgddl.h"
#include "dlvaracc.h"
#include "libkey.h"

/* ----------------------------------------------------------------- */

#define SHIFTSTATUS             0
#define ASCII                   1
#define SCANCODE                2

#define STAT_RSHIFT             0x0000001
#define STAT_LSHIFT             0x0000002
#define STAT_CTRL               0x0000004
#define STAT_ALT                0x0000008
#define STAT_RCTRL              0x0000010
#define STAT_LCTRL              0x0000020
#define STAT_RALT               0x0000040
#define STAT_LALT               0x0000080
#define STAT_NUM                0x0000100
#define STAT_CAPS               0x0000200
#define STAT_SHIFT              0x0000400

/* ---------------------------------------------------------------------- */

typedef struct
{
    SDLKey sym;   /* SDL virtual keysym */
    SDLMod mod;   /* current key modifiers */
    HOTKEY_CALLBACK callback;
} hotkey ;

static hotkey * hotkey_list = NULL ;
static int hotkey_allocated = 0 ;
static int hotkey_count = 0 ;

/* ----------------------------------------------------------------- */

/* Publicas */
key_equiv key_table[127] ;              /* Now we have a search table with equivs */
unsigned char * keystate = NULL;        /* Pointer to key states */

/* ----------------------------------------------------------------- */

static int sdl_equiv[SDLK_LAST+1] ;

static int equivs[] =
{
    SDLK_ESCAPE,        1,
    SDLK_1,             2,
    SDLK_KP1,           2,
    SDLK_2,             3,
    SDLK_KP2,           3,
    SDLK_3,             4,
    SDLK_KP3,           4,
    SDLK_4,             5,
    SDLK_KP4,           5,
    SDLK_5,             6,
    SDLK_KP5,           6,
    SDLK_6,             7,
    SDLK_KP6,           7,
    SDLK_7,             8,
    SDLK_KP7,           8,
    SDLK_8,             9,
    SDLK_KP8,           9,
    SDLK_9,             10,
    SDLK_KP9,           10,
    SDLK_0,             11,
    SDLK_KP0,           11,
    SDLK_MINUS,         12,
    SDLK_EQUALS,        13,
    SDLK_BACKSPACE,     14,
    SDLK_TAB,           15,
    SDLK_q,             16,
    SDLK_w,             17,
    SDLK_e,             18,
    SDLK_r,             19,
    SDLK_t,             20,
    SDLK_y,             21,
    SDLK_u,             22,
    SDLK_i,             23,
    SDLK_o,             24,
    SDLK_p,             25,
    SDLK_LEFTBRACKET,   26,
    SDLK_RIGHTBRACKET,  27,
    SDLK_RETURN,        28,
    SDLK_KP_ENTER,      28,
    SDLK_LCTRL,         29,
    SDLK_RCTRL,         29,
    SDLK_LCTRL,         96,
    SDLK_RCTRL,         94,
    SDLK_a,             30,
    SDLK_s,             31,
    SDLK_d,             32,
    SDLK_f,             33,
    SDLK_g,             34,
    SDLK_h,             35,
    SDLK_j,             36,
    SDLK_k,             37,
    SDLK_l,             38,
    SDLK_SEMICOLON,     39,
    SDLK_QUOTE,         40,
    SDLK_BACKQUOTE,     41,
    SDLK_LSHIFT,        42,
    SDLK_BACKSLASH,     43,
    SDLK_z,             44,
    SDLK_x,             45,
    SDLK_c,             46,
    SDLK_v,             47,
    SDLK_b,             48,
    SDLK_n,             49,
    SDLK_m,             50,
    SDLK_COMMA,         51,
    SDLK_PERIOD,        52,
    SDLK_KP_PERIOD,     52,
    SDLK_SLASH,         53,
    SDLK_KP_DIVIDE,     53,
    SDLK_RSHIFT,        54,
    /*  SDLK_PRINT,         55,*/
    SDLK_KP_MULTIPLY,   55,
    SDLK_LALT,          56,
    SDLK_RALT,          56,
    SDLK_LALT,          95,
    SDLK_RALT,          93,
    SDLK_MODE,          93,
    SDLK_MODE,          56,
    SDLK_RMETA,         56,
    SDLK_LMETA,         56,
    SDLK_SPACE,         57,
    SDLK_CAPSLOCK,      58,
    SDLK_F1,            59,
    SDLK_F2,            60,
    SDLK_F3,            61,
    SDLK_F4,            62,
    SDLK_F5,            63,
    SDLK_F6,            64,
    SDLK_F7,            65,
    SDLK_F8,            66,
    SDLK_F9,            67,
    SDLK_F10,           68,
    SDLK_NUMLOCK,       69,
    SDLK_SCROLLOCK,     70,
    SDLK_HOME,          71,
    SDLK_UP,            72,
    SDLK_PAGEUP,        73,
    SDLK_KP_MINUS,      74,
    SDLK_LEFT,          75,
    SDLK_RIGHT,         77,
    SDLK_KP_PLUS,       78,
    SDLK_END,           79,
    SDLK_DOWN,          80,
    SDLK_PAGEDOWN,      81,
    SDLK_INSERT,        82,
    SDLK_DELETE,        83,
    SDLK_F11,           87,
    SDLK_F12,           88,
    SDLK_LESS,          89,
    SDLK_PLUS,          90,
    SDLK_GREATER,       91,
    SDLK_QUESTION,      92,
    SDLK_CARET,         93,
    SDLK_SYSREQ,        55,
    SDLK_PAUSE,         95,
    SDLK_MENU,          97,
    SDLK_LSUPER,        98,
    SDLK_RSUPER,        99,
    SDLK_COMPOSE,       99,
    -1, -1
} ;

/* ----------------------------------------------------------------- */

DLCONSTANT  __bgdexport( libkey, constants_def )[] =
{
    { "_ESC",         TYPE_DWORD,  1 },
    { "_1",           TYPE_DWORD,  2 },
    { "_2",           TYPE_DWORD,  3 },
    { "_3",           TYPE_DWORD,  4 },
    { "_4",           TYPE_DWORD,  5 },
    { "_5",           TYPE_DWORD,  6 },
    { "_6",           TYPE_DWORD,  7 },
    { "_7",           TYPE_DWORD,  8 },
    { "_8",           TYPE_DWORD,  9 },
    { "_9",           TYPE_DWORD,  10 },
    { "_0",           TYPE_DWORD,  11 },
    { "_MINUS",       TYPE_DWORD,  12 },
    { "_PLUS",        TYPE_DWORD,  13 },
    { "_BACKSPACE",   TYPE_DWORD,  14 },
    { "_TAB",         TYPE_DWORD,  15 },
    { "_Q",           TYPE_DWORD,  16 },
    { "_W",           TYPE_DWORD,  17 },
    { "_E",           TYPE_DWORD,  18 },
    { "_R",           TYPE_DWORD,  19 },
    { "_T",           TYPE_DWORD,  20 },
    { "_Y",           TYPE_DWORD,  21 },
    { "_U",           TYPE_DWORD,  22 },
    { "_I",           TYPE_DWORD,  23 },
    { "_O",           TYPE_DWORD,  24 },
    { "_P",           TYPE_DWORD,  25 },
    { "_L_BRACKET",   TYPE_DWORD,  26 },
    { "_R_BRACKET",   TYPE_DWORD,  27 },
    { "_ENTER",       TYPE_DWORD,  28 },
    { "_C_ENTER",     TYPE_DWORD,  28 },
    { "_CONTROL",     TYPE_DWORD,  29 },
    { "_A",           TYPE_DWORD,  30 },
    { "_S",           TYPE_DWORD,  31 },
    { "_D",           TYPE_DWORD,  32 },
    { "_F",           TYPE_DWORD,  33 },
    { "_G",           TYPE_DWORD,  34 },
    { "_H",           TYPE_DWORD,  35 },
    { "_J",           TYPE_DWORD,  36 },
    { "_K",           TYPE_DWORD,  37 },
    { "_L",           TYPE_DWORD,  38 },
    { "_SEMICOLON",   TYPE_DWORD,  39 },
    { "_APOSTROPHE",  TYPE_DWORD,  40 },
    { "_WAVE",        TYPE_DWORD,  41 },
    { "_L_SHIFT",     TYPE_DWORD,  42 },
    { "_BACKSLASH",   TYPE_DWORD,  43 },
    { "_Z",           TYPE_DWORD,  44 },
    { "_X",           TYPE_DWORD,  45 },
    { "_C",           TYPE_DWORD,  46 },
    { "_V",           TYPE_DWORD,  47 },
    { "_B",           TYPE_DWORD,  48 },
    { "_N",           TYPE_DWORD,  49 },
    { "_M",           TYPE_DWORD,  50 },
    { "_COMMA",       TYPE_DWORD,  51 },
    { "_POINT",       TYPE_DWORD,  52 },
    { "_SLASH",       TYPE_DWORD,  53 },
    { "_C_BACKSLASH", TYPE_DWORD,  53 },
    { "_R_SHIFT",     TYPE_DWORD,  54 },
    { "_C_ASTERISK",  TYPE_DWORD,  55 },
    { "_PRN_SCR",     TYPE_DWORD,  55 },
    { "_ALT",         TYPE_DWORD,  56 },
    { "_SPACE",       TYPE_DWORD,  57 },
    { "_CAPS_LOCK",   TYPE_DWORD,  58 },
    { "_F1",          TYPE_DWORD,  59 },
    { "_F2",          TYPE_DWORD,  60 },
    { "_F3",          TYPE_DWORD,  61 },
    { "_F4",          TYPE_DWORD,  62 },
    { "_F5",          TYPE_DWORD,  63 },
    { "_F6",          TYPE_DWORD,  64 },
    { "_F7",          TYPE_DWORD,  65 },
    { "_F8",          TYPE_DWORD,  66 },
    { "_F9",          TYPE_DWORD,  67 },
    { "_F10",         TYPE_DWORD,  68 },
    { "_NUM_LOCK",    TYPE_DWORD,  69 },
    { "_SCROLL_LOCK", TYPE_DWORD,  70 },
    { "_HOME",        TYPE_DWORD,  71 },
    { "_C_HOME",      TYPE_DWORD,  71 },
    { "_UP",          TYPE_DWORD,  72 },
    { "_C_UP",        TYPE_DWORD,  72 },
    { "_PGUP",        TYPE_DWORD,  73 },
    { "_C_PGUP",      TYPE_DWORD,  73 },
    { "_C_MINUS",     TYPE_DWORD,  74 },
    { "_LEFT",        TYPE_DWORD,  75 },
    { "_C_LEFT",      TYPE_DWORD,  75 },
    { "_C_CENTER",    TYPE_DWORD,  76 },
    { "_RIGHT",       TYPE_DWORD,  77 },
    { "_C_RIGHT",     TYPE_DWORD,  77 },
    { "_C_PLUS",      TYPE_DWORD,  78 },
    { "_END",         TYPE_DWORD,  79 },
    { "_C_END",       TYPE_DWORD,  79 },
    { "_DOWN",        TYPE_DWORD,  80 },
    { "_C_DOWN",      TYPE_DWORD,  80 },
    { "_PGDN",        TYPE_DWORD,  81 },
    { "_C_PGDN",      TYPE_DWORD,  81 },
    { "_INS",         TYPE_DWORD,  82 },
    { "_C_INS",       TYPE_DWORD,  82 },
    { "_DEL",         TYPE_DWORD,  83 },
    { "_C_DEL",       TYPE_DWORD,  83 },
    { "_F11",         TYPE_DWORD,  87 },
    { "_F12",         TYPE_DWORD,  88 },
    { "_LESS",        TYPE_DWORD,  89 },
    { "_EQUALS",      TYPE_DWORD,  90 },
    { "_GREATER",     TYPE_DWORD,  91 },
    { "_ASTERISK",    TYPE_DWORD,  92 },
    { "_R_ALT",       TYPE_DWORD,  93 },
    { "_R_CONTROL",   TYPE_DWORD,  94 },
    { "_L_ALT",       TYPE_DWORD,  95 },
    { "_L_CONTROL",   TYPE_DWORD,  96 },
    { "_MENU",        TYPE_DWORD,  97 },
    { "_L_WINDOWS",   TYPE_DWORD,  98 },
    { "_R_WINDOWS",   TYPE_DWORD,  99 },

    { "STAT_RSHIFT",  TYPE_DWORD,  STAT_RSHIFT },
    { "STAT_LSHIFT",  TYPE_DWORD,  STAT_LSHIFT },
    { "STAT_CTRL",    TYPE_DWORD,  STAT_CTRL   },
    { "STAT_ALT",     TYPE_DWORD,  STAT_ALT    },
    { "STAT_RCTRL",   TYPE_DWORD,  STAT_RCTRL  },
    { "STAT_LCTRL",   TYPE_DWORD,  STAT_LCTRL  },
    { "STAT_RALT",    TYPE_DWORD,  STAT_RALT   },
    { "STAT_LALT",    TYPE_DWORD,  STAT_LALT   },
    { "STAT_NUM",     TYPE_DWORD,  STAT_NUM    },
    { "STAT_CAPS",    TYPE_DWORD,  STAT_CAPS   },
    { "STAT_SHIFT",   TYPE_DWORD,  STAT_SHIFT  },

    { NULL          , 0         ,  0           }
} ;

/* ----------------------------------------------------------------- */
/* Definicion de variables globales (usada en tiempo de compilacion) */

char * __bgdexport( libkey, globals_def ) =
    "shift_status;\n"
    "ascii;\n"
    "scan_code;\n";

/* ----------------------------------------------------------------- */
/* Son las variables que se desea acceder.                           */
/* El interprete completa esta estructura, si la variable existe.    */
/* (usada en tiempo de ejecucion)                                    */

DLVARFIXUP  __bgdexport( libkey, globals_fixup )[] =
{
    /* Nombre de variable global, puntero al dato, tamaño del elemento, cantidad de elementos */
    { "shift_status" , NULL, -1, -1 },
    { "ascii"        , NULL, -1, -1 },
    { "scan_code"    , NULL, -1, -1 },
    { NULL           , NULL, -1, -1 }
};

/* ----------------------------------------------------------------- */

static void add_key_equiv( int equiv, int keyconst )
{
    key_equiv * curr = &key_table[keyconst] ;

    if ( curr->next != NULL ) while ( curr->next != NULL ) curr = curr->next ;

    if ( curr->sdlk_equiv != 0 )
    {
        curr->next = malloc( sizeof( key_equiv ) ) ;
        curr = curr->next ;
        curr->next = NULL ;
    }

    curr->sdlk_equiv = equiv ;
}

/* ----------------------------------------------------------------- */

void hotkey_add( int mod, int sym, HOTKEY_CALLBACK callback )
{
    if ( hotkey_count >= hotkey_allocated )
    {
        hotkey_allocated = hotkey_count + 5;
        hotkey_list = realloc( hotkey_list, hotkey_allocated * sizeof( hotkey_list[0] ) );
    }

    if ( !hotkey_list )
    {
        fprintf( stderr, "No memory for alloc hotkey\n" );
        exit( -1 );
    }

    hotkey_list [hotkey_count].mod = mod;
    hotkey_list [hotkey_count].sym = sym;
    hotkey_list [hotkey_count].callback = callback;
    hotkey_count++;
}

/* ----------------------------------------------------------------- */

/*
 *  FUNCTION : process_key_events
 *
 *  Process all pending SDL events, updating all global variables
 *  and handling debug key presses and cannot be changed
 *
 *  PARAMS :
 *      None
 *
 *  RETURN VALUE :
 *      None
 */

static void process_key_events()
{
    SDL_Event e ;
    SDLMod m ;
    int k, asc ;
    int pressed ;
    key_equiv * curr ;
    int keypress ;
    static struct
    {
        int ascii ;
        int scancode ;
    }
    keyring [64] ;
    static int keyring_start = 0, keyring_tail = 0 ;
    int ignore_key, n;

    /* Actualizar eventos */

    keypress = 0 ;
    m = SDL_GetModState() ;

    /* Procesa los eventos pendientes */
    /* Reset ascii and scancode if last key was released... */
    /* must check all the linked equivs */

    pressed = 0 ;
    if ( GLODWORD( libkey,  SCANCODE ) )
    {
        curr = &key_table[GLODWORD( libkey,  SCANCODE )] ;
        while ( curr != NULL && pressed == 0 )
        {
            if ( keystate[curr->sdlk_equiv] ) pressed = 1 ;
            curr = curr->next ;
        }
    }

    if ( !pressed )
    {
        GLODWORD( libkey,  ASCII )     = 0 ;
        GLODWORD( libkey,  SCANCODE )  = 0 ;
    }

    while ( SDL_PeepEvents( &e, 1, SDL_GETEVENT, SDL_EVENTMASK(SDL_KEYDOWN)|SDL_EVENTMASK(SDL_KEYUP) ) > 0 )
    {
        switch ( e.type )
        {
            case SDL_KEYDOWN:
                ignore_key = 0;
                /* KeyDown HotKey */
                if ( hotkey_count )
                    for ( n = 0; n < hotkey_count; n++ )
                    {
                        if ((( hotkey_list[n].mod & e.key.keysym.mod ) == hotkey_list[n].mod ) &&
                                ( !hotkey_list[n].sym || ( hotkey_list[n].sym == e.key.keysym.sym ) ) )
                        {
                            ignore_key = hotkey_list[n].callback( e.key.keysym );
                        }
                    }
                /* KeyDown HotKey */

                if ( ignore_key ) break ;

                /* Almacena la pulsación de la tecla */

                k = sdl_equiv[e.key.keysym.sym];

                m = e.key.keysym.mod ;

                if ( !keypress )
                {
                    GLODWORD( libkey,  SCANCODE )  = k ;
                    if ( e.key.keysym.unicode )
                    {
                        asc = win_to_dos[e.key.keysym.unicode & 0xFF] ;

                        /* ascii mayusculas */
                        if ( asc >= 'a' && asc <= 'z' && ( m & KMOD_LSHIFT || m & KMOD_RSHIFT || keystate[SDLK_CAPSLOCK] ) )
                            asc -= 0x20 ;
                    }
                    else
                    {
                        asc = 0 ; /* NON PRINTABLE */
                    }

                    GLODWORD( libkey,  ASCII ) = asc ;
                    keypress = 1 ;
                }
                else
                {
                    keyring[keyring_tail].scancode = k ;
                    if ( e.key.keysym.unicode )
                    {
                        asc = win_to_dos[e.key.keysym.unicode & 0x7F] ;

                        /*ascii mayusculas */
                        if ( asc >= 'a' && asc <= 'z' && ( m & KMOD_LSHIFT || m & KMOD_RSHIFT || keystate[SDLK_CAPSLOCK] ) )
                            asc -= 0x20 ;
                    }
                    else
                    {
                        asc = 0 ; /* NON PRINTABLE */
                    }
                    keyring[keyring_tail].ascii = asc ;
                    if ( ++keyring_tail == 64 ) keyring_tail = 0 ;
                }

                break ;

            case SDL_KEYUP:
                /* Do nothing, Bennu is key_up unsensitive */
                break ;
        }
    }

    if ( !keypress && keyring_start != keyring_tail )
    {
        GLODWORD( libkey,  ASCII )     = keyring[keyring_start].ascii ;
        GLODWORD( libkey,  SCANCODE )  = keyring[keyring_start].scancode ;
        if ( ++keyring_start == 64 ) keyring_start = 0 ;
    }

    /* Now actualized every frame... */
    GLODWORD( libkey,  SHIFTSTATUS ) =
        ( ( m & KMOD_RSHIFT ) ? STAT_RSHIFT : 0 ) |
        ( ( m & KMOD_LSHIFT ) ? STAT_LSHIFT : 0 ) |

        ( ( m & KMOD_CTRL   ) ? STAT_CTRL   : 0 ) |
        ( ( m & KMOD_ALT    ) ? STAT_ALT    : 0 ) |

        ( ( m & KMOD_RCTRL  ) ? STAT_RCTRL  : 0 ) |
        ( ( m & KMOD_LCTRL  ) ? STAT_LCTRL  : 0 ) |

        ( ( m & KMOD_RALT   ) ? STAT_RALT   : 0 ) |
        ( ( m & KMOD_LALT   ) ? STAT_LALT   : 0 ) |

        ( ( m & KMOD_NUM    ) ? STAT_NUM    : 0 ) |
        ( ( m & KMOD_CAPS   ) ? STAT_CAPS   : 0 ) |
        ( ( m & KMOD_SHIFT  ) ? STAT_SHIFT  : 0 ) ;
}

/* ----------------------------------------------------------------- */

/* Bigest priority first execute
   Lowest priority last execute */

HOOK __bgdexport( libkey, handler_hooks )[] =
{
    { 4900, process_key_events },
    {    0, NULL               }
} ;

/* ----------------------------------------------------------------- */
/* Funciones de inicializacion del modulo/plugin                     */

void __bgdexport( libkey, module_initialize )()
{
    int * ptr = equivs ;

    if ( !SDL_WasInit( SDL_INIT_VIDEO ) ) SDL_InitSubSystem( SDL_INIT_VIDEO );

    memset( sdl_equiv, 0, sizeof( sdl_equiv ) );
    memset( key_table, 0, sizeof( key_table ) ) ;

    while ( *ptr != -1 )
    {
        sdl_equiv[*ptr] = ptr[1] ;
        add_key_equiv( ptr[0], ptr[1] ) ;
        ptr += 2 ;
    }

    if ( !keystate ) keystate = SDL_GetKeyState( NULL );

    SDL_EnableUNICODE( 1 );
}

/* ----------------------------------------------------------------- */

void __bgdexport( libkey, module_finalize )()
{
    /* FREE used key_equivs... note base 127 equivs are static not allocated... */
    int i ;
    key_equiv * aux ;
    key_equiv * curr = key_table ;

    for ( i = 0;i < 127;i++ )
    {
        if ( curr->next != NULL )
        {
            curr = curr->next ;
            while ( curr->next != NULL )
            {
                aux = curr ;
                curr = curr->next;
                free( aux ) ;
            }
            free( curr ) ;
        }
    }

    if ( SDL_WasInit( SDL_INIT_VIDEO ) ) SDL_QuitSubSystem( SDL_INIT_VIDEO );
}

/* ----------------------------------------------------------------- */

char * __bgdexport( libkey, modules_dependency )[] =
{
    "libsdlhandler",
    NULL
};

/* ----------------------------------------------------------------- */
