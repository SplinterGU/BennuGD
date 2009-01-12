/*
 *  Copyright © 2006-2009 SplinterGU (Fenix/Bennugd)
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

#ifndef __BGDDL_H
#define __BGDDL_H

/* --------------------------------------------------------------------------- */

#ifndef __BGDC__

#ifdef WIN32
#define DLLEXPORT   __declspec(dllexport)
#define DLLIMPORT   __declspec(dllimport)
#else
#define DLLEXPORT
#define DLLIMPORT
#endif

/* --------------------------------------------------------------------------- */

#define __bgdexport(m,a)    m##_##a

/* --------------------------------------------------------------------------- */

#include <typedef_st.h>
#include <instance_st.h>
#include <sysprocs_st.h>

#include <commons_defs.h>

#endif

/* --------------------------------------------------------------------------- */

typedef struct
{
    char * var;
    void * data_offset;
    int size;
    int elements;
} DLVARFIXUP;

typedef struct
{
    char * name;
    char * paramtypes;
    int type;
    void * func;
} DLSYSFUNCS;

typedef struct
{
    char * name;
    BASETYPE type;
    int code;
} DLCONSTANT;

/* --------------------------------------------------------------------------- */

#endif
