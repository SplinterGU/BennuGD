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

#ifndef _LIBKEY_H
    #define _LIBKEY_H

    #include <SDL.h>

    typedef struct _keyequiv {
        int                 sdlk_equiv ;
        struct _keyequiv    * next ;
    } key_equiv ;

    typedef int (* HOTKEY_CALLBACK) (SDL_keysym);

    #ifndef _LIBKEY_C
    extern DLLIMPORT void hotkey_add(int mod, int sym, HOTKEY_CALLBACK callback);

    extern DLLIMPORT key_equiv key_table[] ;           /* Now we have a search table with equivs */
    extern DLLIMPORT unsigned char * keystate ;        /* Pointer to key states */
    #endif

#endif
