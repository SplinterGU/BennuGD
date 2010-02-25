/*
 *  Copyright © 2001, 2002, 2003, 2004 Andrea Mazzoleni
 *
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

/*
 * You can find an high level description of the effect at :
 *
 * http://scale2x.sourceforge.net/scale2x.html
 *
 * Alternatively at the previous license terms, you are allowed to use this
 * code in your program with these conditions:
 * - the program is not used in commercial activities.
 * - the whole source code of the program is released with the binary.
 * - derivative works of the program are allowed.
 */

/* --------------------------------------------------------------------------- */

#define __LIB_RENDER
#include "librender.h"

/* --------------------------------------------------------------------------- */

static void internal_scale2x_16_def( uint16_t *dst0, uint16_t* dst1, const uint16_t* src0, const uint16_t* src1, const uint16_t* src2, unsigned count )
{
    /* first pixel */
    dst0[0] = src1[0];
    dst1[0] = src1[0];

    if ( src1[1] == src0[0] && src2[0] != src0[0] )
        dst0[1] = src0[0];
    else
        dst0[1] = src1[0];

    if ( src1[1] == src2[0] && src0[0] != src2[0] )
        dst1[1] = src2[0];
    else
        dst1[1] = src1[0];

    ++src0;
    ++src1;
    ++src2;

    dst0 += 2;
    dst1 += 2;

    /* central pixels */
    count -= 2;
    while ( count )
    {
        if ( src1[-1] == src0[0] && src2[0] != src0[0] && src1[1] != src0[0] )
            dst0[0] = src0[0];
        else
            dst0[0] = src1[0];

        if ( src1[1] == src0[0] && src2[0] != src0[0] && src1[-1] != src0[0] )
            dst0[1] = src0[0];
        else
            dst0[1] = src1[0];

        if ( src1[-1] == src2[0] && src0[0] != src2[0] && src1[1] != src2[0] )
            dst1[0] = src2[0];
        else
            dst1[0] = src1[0];

        if ( src1[1] == src2[0] && src0[0] != src2[0] && src1[-1] != src2[0] )
            dst1[1] = src2[0];
        else
            dst1[1] = src1[0];

        ++src0;
        ++src1;
        ++src2;

        dst0 += 2;
        dst1 += 2;

        --count;
    }

    /* last pixel */
    if ( src1[-1] == src0[0] && src2[0] != src0[0] )
        dst0[0] = src0[0];
    else
        dst0[0] = src1[0];

    if ( src1[-1] == src2[0] && src0[0] != src2[0] )
        dst1[0] = src2[0];
    else
        dst1[0] = src1[0];

    dst0[1] = src1[0];
    dst1[1] = src1[0];
}

/* --------------------------------------------------------------------------- */

void scale2x( uint8_t *srcPtr, uint32_t srcPitch, uint8_t *dstPtr, uint32_t dstPitch, int width, int height )
{
    uint16_t *dst0 = ( uint16_t * )dstPtr;
    uint16_t *dst1 = dst0 + ( dstPitch / 2 );

    uint16_t *src0 = ( uint16_t * )srcPtr;
    uint16_t *src1 = src0 + ( srcPitch / 2 );
    uint16_t *src2 = src1 + ( srcPitch / 2 );

    int count;

    internal_scale2x_16_def( dst0, dst1, src0, src0, src1, width );

    count = height;

    count -= 2;
    while ( count )
    {
        dst0 += dstPitch;
        dst1 += dstPitch;

        internal_scale2x_16_def( dst0, dst1, src0, src1, src2, width );

        src0 = src1;
        src1 = src2;
        src2 += srcPitch / 2;

        --count;
    }
    dst0 += dstPitch;
    dst1 += dstPitch;

    internal_scale2x_16_def( dst0, dst1, src0, src1, src1, width );
}

/* --------------------------------------------------------------------------- */

