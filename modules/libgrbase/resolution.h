/*
*  Copyright © 2006-2010 SplinterGU (Fenix/Bennugd)
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

#ifndef __RESOLUTION_H
#define __RESOLUTION_H

/* -------------------------------------------------------------------- */
/* Librería gráfica                                                     */
/* -------------------------------------------------------------------- */

#define RESOLXY(m,r,x,y) \
    {                                           \
        int res = LOCINT32(m, r, RESOLUTION ) ; \
        if ( res > 0 )                          \
        {                                       \
            if ( x < 0 )                        \
                ( x ) = ( ( x ) - ( res - 1 )) / res ; \
            else                                \
                ( x ) /= res ;                  \
                                                \
            if ( y < 0 )                        \
                ( y ) = ( ( y ) - ( res - 1 )) / res ; \
            else                                \
                ( y ) /= res ;                  \
        }                                       \
        else if ( res < 0 )                     \
        {                                       \
            ( x ) *= -res ;                     \
            ( y ) *= -res ;                     \
        }                                       \
    }

#define RESOLXYZ(m,r,x,y,z) \
    {                                           \
        int res = LOCINT32(m, r, RESOLUTION ) ; \
        if ( res > 0 )                          \
        {                                       \
            if ( x < 0 )                        \
                ( x ) = ( ( x ) - ( res - 1 )) / res ; \
            else                                \
                ( x ) /= res ;                  \
                                                \
            if ( y < 0 )                        \
                ( y ) = ( ( y ) - ( res - 1 )) / res ; \
            else                                \
                ( y ) /= res ;                  \
                                                \
            if ( z < 0 )                        \
                ( z ) = ( ( z ) - ( res - 1 )) / res ; \
            else                                \
                ( z ) /= res ;                  \
        }                                       \
        else if ( res < 0 )                     \
        {                                       \
            ( x ) *= -res ;                     \
            ( y ) *= -res ;                     \
            ( z ) *= -res ;                     \
        }                                       \
    }

#endif
