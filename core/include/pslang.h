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

#ifndef __PSLANG_H
#define __PSLANG_H

/* --------------------- */
/* Flags para mnemónicos */
/* --------------------- */

/* Número de parámetros */

#define MN_0_PARAMS     (0x00)
#define MN_1_PARAMS     (0x80)

#define MN_PARAMS(code) (((code) & 0x80) >> 7)

/* Mascara */
#define MN_MASK         0xFF

/* Tipo de dato */

#define MN_DWORD        (0x000)
#define MN_WORD         (0x100)
#define MN_BYTE         (0x200)
#define MN_STRING       (0x300)
#define MN_FLOAT        (0x400)
#define MN_UNSIGNED     (0x800)

#define MN_TYPEOF(code) ((code) & 0xF00)

/* -------------------- */
/* Códigos de mnemónico */
/* -------------------- */

/* Salidas */

#define MN_END                  (0x00| MN_0_PARAMS)
#define MN_RETURN               (0x01| MN_0_PARAMS)
#define MN_FRAME                (0x02| MN_0_PARAMS)

/* Operaciones con la pila */

#define MN_DUP                  (0x03| MN_0_PARAMS)
#define MN_PUSH                 (0x04| MN_1_PARAMS)
#define MN_POP                  (0x05| MN_0_PARAMS)

/* Control Switch */

#define MN_SWITCH               (0x06| MN_0_PARAMS)
#define MN_CASE                 (0x07| MN_0_PARAMS)
#define MN_CASE_R               (0x08| MN_0_PARAMS)

/* Llamadas */

#define MN_CALL                 (0x09| MN_1_PARAMS)
#define MN_SYSCALL              (0x0A| MN_1_PARAMS)
#define MN_PROC                 (0x0B| MN_1_PARAMS)
#define MN_SYSPROC              (0x0C| MN_1_PARAMS)
#define MN_CLONE                (0x0D| MN_1_PARAMS)

#define MN_TYPE                 (0x0E| MN_1_PARAMS)
#define MN_DEBUG                (0x0F| MN_0_PARAMS)

/* Direccionamiento */

#define MN_INDEX                (0x10| MN_1_PARAMS)
#define MN_ARRAY                (0x11| MN_1_PARAMS)
#define MN_PRIVATE              (0x12| MN_1_PARAMS)
#define MN_LOCAL                (0x13| MN_1_PARAMS)
#define MN_GLOBAL               (0x14| MN_1_PARAMS)
#define MN_REMOTE               (0x15| MN_1_PARAMS)
#define MN_PTR                  (0x16| MN_0_PARAMS)

/* Acceso a variables */

#define MN_GET_PRIV             (0x17| MN_1_PARAMS)
#define MN_GET_LOCAL            (0x18| MN_1_PARAMS)
#define MN_GET_GLOBAL           (0x19| MN_1_PARAMS)
#define MN_GET_REMOTE           (0x1A| MN_1_PARAMS)

/* Control de flujo */

#define MN_JUMP                 (0x1B| MN_1_PARAMS)
#define MN_JFALSE               (0x1C| MN_1_PARAMS)
#define MN_JTRUE                (0x1D| MN_1_PARAMS)
#define MN_JTFALSE              (0x1E| MN_1_PARAMS)
#define MN_JTTRUE               (0x1F| MN_1_PARAMS)
#define MN_JNOCASE              (0x20| MN_1_PARAMS)

/* Operaciones unarias - aceptan tipos de dato numéricos */

#define MN_NEG                  (0x21| MN_0_PARAMS)
#define MN_NOT                  (0x22| MN_0_PARAMS)

/* Operaciones binarias - Aceptan tipos de dato numéricos */

#define MN_MUL                  (0x23| MN_0_PARAMS)
#define MN_DIV                  (0x24| MN_0_PARAMS)
#define MN_ADD                  (0x25| MN_0_PARAMS)
#define MN_SUB                  (0x26| MN_0_PARAMS)
#define MN_MOD                  (0x27| MN_0_PARAMS)
#define MN_ROR                  (0x28| MN_0_PARAMS)
#define MN_ROL                  (0x29| MN_0_PARAMS)
#define MN_AND                  (0x2A| MN_0_PARAMS)
#define MN_OR                   (0x2B| MN_0_PARAMS)
#define MN_XOR                  (0x2C| MN_0_PARAMS)

/* Comparaciones binarias */

#define MN_EQ                   (0x2D| MN_0_PARAMS)
#define MN_NE                   (0x2E| MN_0_PARAMS)
#define MN_GT                   (0x2F| MN_0_PARAMS)
#define MN_LT                   (0x30| MN_0_PARAMS)
#define MN_GTE                  (0x31| MN_0_PARAMS)
#define MN_LTE                  (0x32| MN_0_PARAMS)

/* Post/pre-operadores */

#define MN_POSTINC              (0x33| MN_1_PARAMS)
#define MN_POSTDEC              (0x34| MN_1_PARAMS)
#define MN_INC                  (0x35| MN_1_PARAMS)
#define MN_DEC                  (0x36| MN_1_PARAMS)

/* Alteración directa de variables */

#define MN_LET                  (0x37| MN_0_PARAMS)
#define MN_VARADD               (0x38| MN_0_PARAMS)
#define MN_VARSUB               (0x39| MN_0_PARAMS)
#define MN_VARMUL               (0x3A| MN_0_PARAMS)
#define MN_VARDIV               (0x3B| MN_0_PARAMS)
#define MN_VARMOD               (0x3C| MN_0_PARAMS)
#define MN_VARXOR               (0x3D| MN_0_PARAMS)
#define MN_VARAND               (0x3E| MN_0_PARAMS)
#define MN_VAROR                (0x3F| MN_0_PARAMS)
#define MN_VARROR               (0x40| MN_0_PARAMS)
#define MN_VARROL               (0x41| MN_0_PARAMS)

/* Funciones de cadena */

#define MN_SUBSTR               (0x42| MN_0_PARAMS)
#define MN_CHRSTR               (0x43| MN_0_PARAMS)
#define MN_INT2STR              (0x44| MN_1_PARAMS)
#define MN_FLOAT2STR            (0x45| MN_1_PARAMS)
#define MN_CHR2STR              (0x46| MN_1_PARAMS)

/* Optimizaciones */

#define MN_LETNP                (0x47| MN_0_PARAMS)

/* Operadores Bitwise */

#define MN_BNOT                 (0x48| MN_0_PARAMS)
#define MN_BAND                 (0x49| MN_0_PARAMS)
#define MN_BOR                  (0x4A| MN_0_PARAMS)
#define MN_BXOR                 (0x4B| MN_0_PARAMS)

/* Funciones de conversión */

#define MN_INT2FLOAT            (0x50| MN_1_PARAMS)
#define MN_FLOAT2INT            (0x51| MN_1_PARAMS)

#define MN_A2STR                (0x60| MN_1_PARAMS)
#define MN_STR2A                (0x61| MN_1_PARAMS)
#define MN_STRACAT              (0x62| MN_1_PARAMS)
#define MN_INT2WORD             (0x63| MN_1_PARAMS)
#define MN_INT2BYTE             (0x64| MN_1_PARAMS)

/* Funciones de control de flujo */
#define MN_NCALL                (0x65| MN_1_PARAMS)

/* Handlers */
#define MN_EXITHNDLR            (0x6A| MN_1_PARAMS)
#define MN_ERRHNDLR             (0x6B| MN_1_PARAMS)
#define MN_RESUME               (0x6C| MN_1_PARAMS)
#define MN_RESUMEGOTO           (0x6D| MN_1_PARAMS)

/* Mnemónicos inexistentes - usados por el compilador internamente */

#define MN_REPEAT               (0x70| MN_1_PARAMS)
#define MN_BREAK                (0x71| MN_1_PARAMS)
#define MN_BRFALSE              (0x72| MN_1_PARAMS)
#define MN_RETRUE               (0x73| MN_1_PARAMS)
#define MN_REFALSE              (0x74| MN_1_PARAMS)

/* Funciones de cadena (2) */

#define MN_POINTER2STR          (0x75| MN_1_PARAMS)
/* #define MN_POINTER2BOL          (0x76| MN_1_PARAMS) */
#define MN_STR2INT              (0x77| MN_1_PARAMS)
#define MN_STR2FLOAT            (0x78| MN_1_PARAMS)
#define MN_STR2CHR              (0x79| MN_1_PARAMS)

/* Tratamiento de Locales a proceso */

#define MN_PUBLIC               (0x7A| MN_1_PARAMS)     /* Direccionamiento */
#define MN_GET_PUBLIC           (0x7B| MN_1_PARAMS)     /* Acceso a variables */

#define MN_REMOTE_PUBLIC        (0x7C| MN_1_PARAMS)     /* Direccionamiento */
#define MN_GET_REMOTE_PUBLIC    (0x7D| MN_1_PARAMS)     /* Acceso a variables */

/* Depurado */

#define MN_SENTENCE             (0x7F| MN_1_PARAMS)

/* Max: 0x7F */

#endif
