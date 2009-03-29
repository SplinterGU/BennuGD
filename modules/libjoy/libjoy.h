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

#ifndef __LIBJOY_H
#define __LIBJOY_H

extern int libjoy_num( void );
extern int libjoy_name( int joy );
extern int libjoy_select( int joy );
extern int libjoy_buttons( void );
extern int libjoy_axes( void );
extern int libjoy_get_button( int button );
extern int libjoy_get_position( int axis );
extern int libjoy_hats( void );
extern int libjoy_balls( void );
extern int libjoy_get_hat( int hat );
extern int libjoy_get_ball( int ball, int * dx, int * dy );
extern int libjoy_buttons_specific( int joy );
extern int libjoy_axes_specific( int joy );
extern int libjoy_get_button_specific( int joy, int button );
extern int libjoy_get_position_specific( int joy, int axis );
extern int libjoy_hats_specific( int joy );
extern int libjoy_balls_specific( int joy );
extern int libjoy_get_hat_specific( int joy, int hat );
extern int libjoy_get_ball_specific( int joy, int ball, int * dx, int * dy );

#endif
