/*
 *  Copyright � 2006-2019 SplinterGU (Fenix/Bennugd)
 *  Copyright � 2002-2006 Fenix Team (Fenix)
 *  Copyright � 1999-2002 Jos� Luis Cebri�n Pag�e (Fenix)
 *
 *  This file is part of Bennu - Game Development
 *
 *  This software is provided 'as-is', without any express or implied
 *  warranty. In no event will the authors be held liable for any damages
 *  arising from the use of this software.
 *
 *  Permission is granted to anyone to use this software for any purpose,
 *  including commercial applications, and to alter it and redistribute it
 *  freely, subject to the following restrictions:
 *
 *     1. The origin of this software must not be misrepresented; you must not
 *     claim that you wrote the original software. If you use this software
 *     in a product, an acknowledgment in the product documentation would be
 *     appreciated but is not required.
 *
 *     2. Altered source versions must be plainly marked as such, and must not be
 *     misrepresented as being the original software.
 *
 *     3. This notice may not be removed or altered from any source
 *     distribution.
 *
 */

#ifndef __SYSPROCS_ST_H
#define __SYSPROCS_ST_H

/* --------------------------------------------------------------------------- */

#ifndef __BGDC__
#include <instance.h>
#endif

/* ---------------------------------------------------------------------- */

typedef void ( * FN_HOOK )( void );
typedef void ( * INSTANCE_HOOK )( INSTANCE * );
typedef void ( * CFG_HOOK )( int, const char *, const char *, const char * );

typedef struct
{
    int prio;
    FN_HOOK hook;
} HOOK ;

/* ---------------------------------------------------------------------- */

#endif
