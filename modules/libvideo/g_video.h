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

#ifndef __G_VIDEO_H
#define __G_VIDEO_H

#ifdef TARGET_MAC
#include <SDL/SDL.h>
#else
#include <SDL.h>
#endif

#include "libgrbase.h"

/* --------------------------------------------------------------------------- */
/* Graph Mode */
#define MODE_WINDOW         0x0000
#define MODE_16BITS         0x0010
#define MODE_32BITS         0x0020
#define MODE_2XSCALE        0x0100
#define MODE_FULLSCREEN     0x0200
#define MODE_DOUBLEBUFFER   0x0400
#define MODE_HARDWARE       0x0800
#define MODE_MODAL          0x1000
#define MODE_FRAMELESS      0x2000
#define MODE_WAITVSYNC      0x4000

/* Scaler */
#define SCALE_NONE          0x0000
#define SCALE_SCALE2X       0x0001

/* --------------------------------------------------------------------------- */

extern GRAPH * icon ;

extern SDL_Surface * screen ;

extern char * apptitle ;

extern int scr_width ;
extern int scr_height ;

extern int scr_initialized ;

extern int enable_16bits ;
extern int enable_scale ;
extern int full_screen ;
extern int double_buffer ;
extern int hardware_scr ;
extern int grab_input ;
extern int frameless ;
extern int scale_mode ;

extern int waitvsync ;

/* --------------------------------------------------------------------------- */

extern int gr_init( int width, int height );
extern void gr_set_caption( char * title );
extern int gr_set_icon( GRAPH * map );
extern int gr_set_mode( int width, int height, int depth );
extern void gr_wait_vsync();

/* --------------------------------------------------------------------------- */

#endif
