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

/* By Splinter 2007 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>

/* --------------------------------------------------------------------------- */

#define __LIB_RENDER
#include "librender.h"

/* --------------------------------------------------------------------------- */

void scanline2x( uint8_t *srcPtr, uint32_t srcPitch, uint8_t *dstPtr, uint32_t dstPitch, int width, int height )
{
    const uint32_t nextlineSrc = srcPitch / sizeof( uint16_t );
    const uint16_t *p = ( const uint16_t * )srcPtr;

    const uint32_t nextlineDst = dstPitch / sizeof( uint16_t );
    uint16_t *q = ( uint16_t * )dstPtr;

    while ( height-- )
    {
        int tmpWidth = width;
        while ( tmpWidth-- )
        {
            *q = *p;
            *( q + nextlineDst ) = 0;
            q++;
            *q = *p;
            *( q + nextlineDst ) = 0;
            q++;
            p++;
        }
        p += nextlineSrc - width;
        q += ( nextlineDst - width ) * 2;
    }
}
