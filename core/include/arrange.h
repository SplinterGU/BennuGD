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

#ifndef __ARRANGE_ST_H
    #define __ARRANGE_ST_H

    #define __LIL_ENDIAN 1234
    #define __BIG_ENDIAN 4321

    #if defined(__hppa__) || \
        defined(__m68k__) || \
        defined(mc68000) || \
        defined(_M_M68K) || \
        (defined(__MIPS__) && defined(__MISPEB__)) || \
        defined(__ppc__) || \
        defined(__POWERPC__) || \
        defined(_M_PPC) || \
        defined(__sparc__)
        #define __BYTEORDER  __BIG_ENDIAN
    #else
        #define __BYTEORDER  __LIL_ENDIAN
    #endif

    /* ---------------------------------------------------------------------- */
    /* Trucos de portabilidad                                                 */
    /* ---------------------------------------------------------------------- */

    #if __BYTEORDER == __LIL_ENDIAN
        #define ARRANGE_DWORD(x)
        #define ARRANGE_WORD(x)

        #define ARRANGE_DWORDS(x,c)
        #define ARRANGE_WORDS(x,c)
    #else
        static __inline__ void DO_Swap16(uint16_t * D) {
            *D = ((*D<<8)|(*D>>8));
        }

        static __inline__ void DO_Swap32(uint32_t * D) {
            *D = ((*D<<24)|((*D<<8)&0x00FF0000)|((*D>>8)&0x0000FF00)|(*D>>24));
        }

        #define ARRANGE_DWORD(x)    DO_Swap32(x)
        #define ARRANGE_WORD(x)     DO_Swap16(x)

        #define ARRANGE_DWORDS(x,c) {               \
            int __n;                                \
            uint32_t * __p = (uint32_t *)(x);       \
            for (__n = 0 ; __n < (int)(c) ; __n++)  \
                ARRANGE_DWORD(&__p[__n]);           \
            }
        #define ARRANGE_WORDS(x,c) {                \
            int __n;                                \
            uint16_t * __p = (uint16_t *)(x);       \
            for (__n = 0 ; __n < (int)(c) ; __n++)  \
                ARRANGE_WORD(&__p[__n]);            \
            }
    #endif
#endif
