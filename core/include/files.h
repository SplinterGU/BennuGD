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

#ifndef __FILES_H
#define __FILES_H

#include <stdint.h>

#include "files_st.h"

/* Funciones de acceso a ficheros */
/* ------------------------------ */

/* Ahora mismo son casi wrappers de stdio.h, pero en el futuro
 * el tipo "file" puede ser una estructura y las funciones,
 * ofrecer soporte transparente para ficheros PAK, etc. */

extern file * file_open        (const char * filename, char * mode) ;

extern int    file_read        (file * fp, void * buffer, int len) ;

extern int    file_readSint8   (file * fp, int8_t  * buffer) ;
extern int    file_readUint8   (file * fp, uint8_t  * buffer) ;
extern int    file_readSint16  (file * fp, int16_t * buffer) ;
extern int    file_readUint16  (file * fp, uint16_t * buffer) ;
extern int    file_readSint32  (file * fp, int32_t * buffer) ;
extern int    file_readUint32  (file * fp, uint32_t * buffer) ;

extern int    file_readSint8A  (file * fp, int8_t  * buffer, int n) ;
extern int    file_readUint8A  (file * fp, uint8_t  * buffer, int n) ;
extern int    file_readSint16A (file * fp, int16_t * buffer, int n) ;
extern int    file_readUint16A (file * fp, uint16_t * buffer, int n) ;
extern int    file_readSint32A (file * fp, int32_t * buffer, int n) ;
extern int    file_readUint32A (file * fp, uint32_t * buffer, int n) ;

extern int    file_write       (file * fp, void * buffer, int len) ;

extern int    file_writeSint8  (file * fp, int8_t  * buffer) ;
extern int    file_writeUint8  (file * fp, uint8_t  * buffer) ;
extern int    file_writeSint16 (file * fp, int16_t * buffer) ;
extern int    file_writeUint16 (file * fp, uint16_t * buffer) ;
extern int    file_writeSint32 (file * fp, int32_t * buffer) ;
extern int    file_writeUint32 (file * fp, uint32_t * buffer) ;

extern int    file_writeSint8A (file * fp, int8_t  * buffer, int n) ;
extern int    file_writeUint8A (file * fp, uint8_t  * buffer, int n) ;
extern int    file_writeSint16A(file * fp, int16_t * buffer, int n) ;
extern int    file_writeUint16A(file * fp, uint16_t * buffer, int n) ;
extern int    file_writeSint32A(file * fp, int32_t * buffer, int n) ;
extern int    file_writeUint32A(file * fp, uint32_t * buffer, int n) ;

extern int    file_qgets       (file * fp, char * buffer, int len) ;
extern int    file_qputs       (file * fp, char * buffer) ;
extern int    file_gets        (file * fp, char * buffer, int len) ;
extern int    file_puts        (file * fp, char * buffer) ;
extern int    file_size        (file * fp) ;
extern int    file_pos         (file * fp) ;
extern int    file_seek        (file * fp, int pos, int where) ;
extern void   file_addp        (const char * path) ;
extern void   file_close       (file * fp) ;
extern int    file_exists      (const char * filename) ;
extern void   file_add_xfile   (file * fp, long offset, char * name, int size) ;
extern int    file_eof         (file * fp) ;
extern FILE * file_fp          (file * fp) ;

extern void   xfile_init       (int maxfiles);

extern int    opened_files;

#endif
