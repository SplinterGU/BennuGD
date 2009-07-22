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

/* --------------------------------------------------------------------------- */

#ifndef __COMMONS_DEFS_H
#define __COMMONS_DEFS_H

/* Instances Status, used in local STATUS var */

#define STATUS_DEAD             0
#define STATUS_KILLED           1
#define STATUS_RUNNING          2
#define STATUS_SLEEPING         3
#define STATUS_FROZEN           4
#define STATUS_WAITING_MASK     0x8000

/* New OS_* must be updated in bgdc/src/main_core.c and bgdrtm/src/misc.c */

#define OS_WIN32                0
#define OS_LINUX                1
#define OS_BEOS                 2
#define OS_MACOS                3
#define OS_GP32                 4
#define OS_DC                   5
#define OS_BSD                  6
#define OS_GP2X                 7
#define OS_GP2X_WIZ             8

#endif

/* --------------------------------------------------------------------------- */
