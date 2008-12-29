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

/* --------------------------------------------------------------------------- */

#ifndef _DLVARACC_H
#define _DLVARACC_H

/* --------------------------------------------------------------------------- */

#include <stdint.h>

/* --------------------------------------------------------------------------- */

#undef LOCDWORD
#undef LOCWORD
#undef LOCBYTE

#undef LOCINT32
#undef LOCINT16
#undef LOCINT8

#undef LOCUINT32
#undef LOCUINT16
#undef LOCUINT8


#undef GLODWORD
#undef GLOWORD
#undef GLOBYTE

#undef GLOINT32
#undef GLOINT16
#undef GLOINT8

#undef GLOUINT32
#undef GLOUINT16
#undef GLOUINT8

/* --------------------------------------------------------------------------- */
/* Macros para acceder a datos locales de una instancia */

#define LOCEXISTS(m,a)    (m##_locals_fixup[a].size != -1)
#define GLOEXISTS(m,a)    (m##_globals_fixup[a].data_offset)

#define LOCADDR(m,a,b)    ((uint8_t *)((a)->locdata) + (uint32_t)m##_locals_fixup[b].data_offset)
#define GLOADDR(m,a)      (m##_globals_fixup[a].data_offset)

#define LOCDWORD(m,a,b)   (*(uint32_t *)LOCADDR(m,a,b))
#define LOCWORD(m,a,b)    (*(uint16_t *)LOCADDR(m,a,b))
#define LOCBYTE(m,a,b)    (*(uint8_t  *)LOCADDR(m,a,b))

#define LOCINT32(m,a,b)   (*(int32_t  *)LOCADDR(m,a,b))
#define LOCINT16(m,a,b)   (*(int16_t  *)LOCADDR(m,a,b))
#define LOCINT8(m,a,b)    (*(int8_t   *)LOCADDR(m,a,b))

#define LOCUINT32(m,a,b)  (*(uint32_t *)LOCADDR(m,a,b))
#define LOCUINT16(m,a,b)  (*(uint16_t *)LOCADDR(m,a,b))
#define LOCUINT8(m,a,b)   (*(uint8_t  *)LOCADDR(m,a,b))


#define GLODWORD(m,a)     (*(uint32_t *)GLOADDR(m,a))
#define GLOWORD(m,a)      (*(uint16_t *)GLOADDR(m,a))
#define GLOBYTE(m,a)      (*(uint8_t  *)GLOADDR(m,a))

#define GLOINT32(m,a)     (*(int32_t  *)GLOADDR(m,a))
#define GLOINT16(m,a)     (*(int16_t  *)GLOADDR(m,a))
#define GLOINT8(m,a)      (*(int8_t   *)GLOADDR(m,a))

#define GLOUINT32(m,b)    (*(uint32_t *)GLOADDR(m,b))
#define GLOUINT16(m,b)    (*(uint16_t *)GLOADDR(m,b))
#define GLOUINT8(m,b)     (*(uint8_t  *)GLOADDR(m,b))

/* --------------------------------------------------------------------------- */
#endif
