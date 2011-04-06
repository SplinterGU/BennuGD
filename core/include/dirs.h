/*
 *  Copyright © 2006-2011 SplinterGU (Fenix/Bennugd)
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
 * FILE        : dirs.h
 * DESCRIPTION : Base include for directory functions
 *
 * HISTORY:
 *
 */

#ifndef __DIRS_H
#define __DIRS_H

#ifdef WIN32
#include <windows.h>
#include <winbase.h>
#include <windef.h>
#include <direct.h>
#else
#include <glob.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

#include <errno.h>
#include <time.h>

#include "files_st.h"

/* ------------------------------------------------------------------------------------ */

enum
{
    DIR_FI_ATTR_NORMAL         = 0x00000000,
    DIR_FI_ATTR_DIRECTORY      = 0x00000001,
    DIR_FI_ATTR_HIDDEN         = 0x00000002,
    DIR_FI_ATTR_READONLY       = 0x00000004
};

/* ------------------------------------------------------------------------------------ */

typedef struct __DIR_FILEINFO_ST
{
    char fullpath[__MAX_PATH];
    char filename[__MAX_PATH];
    long attributes;
    long size;
    struct tm creation_time;
    struct tm modified_time;
} __DIR_FILEINFO_ST;

typedef struct __DIR_ST
{
    char * path;
#ifdef _WIN32
    WIN32_FIND_DATA data;
    HANDLE handle;
    int eod;
#else
    glob_t globd;
    int currFile;
    char *pattern;
#endif
    __DIR_FILEINFO_ST info;
} __DIR_ST;

/* ------------------------------------------------------------------------------------ */

extern char *               dir_path_convert(const char *path) ;

extern char *	            dir_current(void) ;
extern int		            dir_change(const char *dir) ;
extern int		            dir_create(const char *dir) ;
extern int		            dir_delete(const char *dir) ;
extern int		            dir_deletefile(const char *filename) ;

extern __DIR_ST *           dir_open( const char * path );
extern void                 dir_close ( __DIR_ST * hDir );
extern __DIR_FILEINFO_ST *  dir_read( __DIR_ST * hDir );

#endif
