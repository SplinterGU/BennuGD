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

/* ------------------------------------------ */
/* Offsets de los datos globales predefinidos */
/* ------------------------------------------ */

#define ARGC                    0
#define ARGV_TABLE              ARGC                    + 4     /* 32+1 cadenas */
#define OS_ID                   ARGV_TABLE              + 33*4

/* ----------------------------------------- */
/* Offsets de los datos locales predefinidos */
/* ----------------------------------------- */

#define PROCESS_ID              0
#define PROCESS_TYPE            PROCESS_ID          + 4
#define FRAME_PERCENT           PROCESS_TYPE        + 4
#define STATUS                  FRAME_PERCENT       + 4
#define SAVED_STATUS            STATUS              + 4
#define SAVED_PRIORITY          SAVED_STATUS        + 4
#define FATHER                  SAVED_PRIORITY      + 4
#define SON                     FATHER              + 4
#define SMALLBRO                SON                 + 4
#define BIGBRO                  SMALLBRO            + 4
#define PRIORITY                BIGBRO              + 4
