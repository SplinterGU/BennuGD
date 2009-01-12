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

#define ISALPHA(c)     (c_type [(unsigned char)c] & CTYPE_ALPHA)
#define ISSPACE(c)     (c_type [(unsigned char)c] & CTYPE_SPACE)
#define ISNUM(c)       (c_type [(unsigned char)c] & CTYPE_NUM)
#define ISALNUM(c)     (c_type [(unsigned char)c] & CTYPE_ALNUM)
#define ISWORDCHAR(c)  (c_type [(unsigned char)c] & CTYPE_WORDCHAR)
#define ISWORDFIRST(c) (c_type [(unsigned char)c] & CTYPE_WORDFIRST)
#define TOUPPER(c)     (c_upper[(unsigned char)c])
#define TOLOWER(c)     (c_lower[(unsigned char)c])

#define CTYPE_ALPHA     1
#define CTYPE_NUM       2
#define CTYPE_WORDCHAR  4
#define CTYPE_WORDFIRST 8
#define CTYPE_SPACE     16
#define CTYPE_ALNUM     32

