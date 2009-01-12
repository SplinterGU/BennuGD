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

#include <stdio.h>

#include "pslang.h"
#include "dcb.h"
#include "xstrings.h"

#ifndef __BGDRTM__
#include "bgdc.h"
#endif

#ifdef __BGDRTM__
#include "sysprocs_p.h"
#endif

/* ---------------------------------------------------------------------- */
/* Este módulo contiene funciones que muestran el equivalente             */
/* en texto de una instrucción o mnemónico                                */
/* ---------------------------------------------------------------------- */

struct
{
    char * name;
    int    code;
    int    params ;
}
mnemonics[] =
{
    { "ARRAY", MN_ARRAY, 1 },
    { "GET_PRIVATE", MN_GET_PRIV, 1 },
    { "GET_REMOTE", MN_GET_REMOTE, 1 },
    { "GET_LOCAL", MN_GET_LOCAL, 1 },
    { "GET_GLOBAL", MN_GET_GLOBAL, 1 },
    { "PRIVATE", MN_PRIVATE, 1 },
    { "REMOTE", MN_REMOTE, 1 },
    { "LOCAL", MN_LOCAL, 1 },
    { "GLOBAL", MN_GLOBAL, 1 },
    { "DUP", MN_DUP },
    { "PUSH", MN_PUSH, 1 },
    { "POP",  MN_POP },
    { "INDEX", MN_INDEX, 1 },
    { "NEG", MN_NEG },
    { "NOT", MN_NOT },
    { "CALL", MN_CALL, 1 },
    { "PROCESS", MN_PROC, 1 },
    { "SYSCALL", MN_SYSCALL, 1 },
    { "SYSPROC", MN_SYSPROC, 1 },
    { "CLONE", MN_CLONE, 1 },
    { "END", MN_END },
    { "RETURN", MN_RETURN },
    { "FRAME", MN_FRAME },
    { "TYPE", MN_TYPE, 1 },

    { "MUL", MN_MUL },
    { "DIV", MN_DIV },
    { "ADD", MN_ADD },
    { "SUB", MN_SUB },
    { "MOD", MN_MOD },
    { "ROR", MN_ROR },
    { "ROL", MN_ROL },
    { "AND", MN_AND },
    { "OR", MN_OR },
    { "XOR", MN_XOR },

    { "BNOT", MN_BNOT },
    { "BAND", MN_BAND },
    { "BOR", MN_BOR },
    { "BXOR", MN_BXOR },

    { "EQ", MN_EQ },
    { "NE", MN_NE },
    { "GT", MN_GT },
    { "LT", MN_LT },
    { "GTE", MN_GTE },
    { "LTE", MN_LTE },

    { "FLOAT2INT", MN_FLOAT2INT, 1 },
    { "INT2FLOAT", MN_INT2FLOAT, 1 },
    { "INT2WORD", MN_INT2WORD, 1 },
    { "INT2BYTE", MN_INT2BYTE, 1 },

    { "POSTINC", MN_POSTINC, 1 },
    { "POSTDEC", MN_POSTDEC, 1 },
    { "INC", MN_INC, 1 },
    { "DEC", MN_DEC, 1 },
    { "VARADD", MN_VARADD },
    { "VARSUB", MN_VARSUB },
    { "VARMUL", MN_VARMUL },
    { "VARDIV", MN_VARDIV },
    { "VARMOD", MN_VARMOD },
    { "VARXOR", MN_VARXOR },
    { "VARAND", MN_VARAND },
    { "VAROR", MN_VAROR },
    { "VARROR", MN_VARROR },
    { "VARROL", MN_VARROL },
    { "LET", MN_LET },
    { "LETNP", MN_LETNP },

    { "REPEAT", MN_REPEAT, 1 },
    { "BREAK", MN_BREAK, 1 },
    { "BRFALSE", MN_BRFALSE, 1 },
    { "RETRUE", MN_RETRUE, 1 },
    { "REFALSE", MN_REFALSE, 1 },
    { "JUMP", MN_JUMP, 1 },
    { "JFALSE", MN_JFALSE, 1 },
    { "JTRUE", MN_JTRUE, 1 },
    { "JTFALSE", MN_JTFALSE, 1 },
    { "JTTRUE", MN_JTTRUE, 1 },
    { "PTR", MN_PTR },

    { "SWITCH", MN_SWITCH },
    { "CASE", MN_CASE, 0 },
    { "CASE_R", MN_CASE_R, 0 },
    { "JNOCASE", MN_JNOCASE, 1 },

    { "CHRSTR", MN_CHRSTR },
    { "SUBSTR", MN_SUBSTR, 1 },

    { "STR2INT", MN_STR2INT, 1 },
    { "STR2CHR", MN_STR2CHR, 1 },
    { "INT2STR", MN_INT2STR, 1 },
    { "CHR2STR", MN_CHR2STR, 1 },
    { "STR2FLOAT", MN_STR2FLOAT, 1 },
    { "FLOAT2STR", MN_FLOAT2STR, 1 },
    { "POINTER2STR", MN_POINTER2STR, 1 },
// { "POINTER2BOL", MN_POINTER2BOL, 1 },

    { "A2STR", MN_A2STR, 0 },
    { "STR2A", MN_STR2A, 0 },
    { "STRACAT", MN_STRACAT, 0 },


    { "MN_NCALL", MN_NCALL, 1},

    { "MN_EXITHNDLR", MN_EXITHNDLR, 1},
    { "MN_ERRHNDLR", MN_ERRHNDLR, 1},
    { "MN_RESUME", MN_RESUME, 1},
    { "MN_RESUMEGOTO", MN_RESUMEGOTO, 1},


    { "DEBUG", MN_DEBUG, 1 },
    { "------", MN_SENTENCE, 1 },

    { "PUBLIC", MN_PUBLIC, 1 },
    { "GET_PUBLIC", MN_GET_PUBLIC, 1 },

    { "REMOTE_PUBLIC", MN_REMOTE_PUBLIC, 1 },
    { "GET_REMOTE_PUBLIC", MN_GET_REMOTE_PUBLIC, 1 },

    { 0, -1 }
} ;

/* ---------------------------------------------------------------------- */

void mnemonic_dump( int i, int param )
{
    int n = 0 ;

    if ( MN_PARAMS( i ) )
        printf( "%08X %08X "    , i, param );
    else
        printf( "%08X          ", i );

    while ( mnemonics[n].name )
    {
        if ( mnemonics[n].code == ( i & MN_MASK ) )
        {
            switch ( MN_TYPEOF( i ) )
            {
                case MN_UNSIGNED:
                    printf( "UNSIGNED " ) ;
                    break ;
                case MN_WORD:
                    printf( "SHORT    " ) ;
                    break ;
                case MN_WORD | MN_UNSIGNED:
                    printf( "WORD     " ) ;
                    break ;
                case MN_BYTE:
                    printf( "CHAR     " ) ;
                    break ;
                case MN_BYTE | MN_UNSIGNED:
                    printf( "BYTE     " ) ;
                    break ;
                case MN_FLOAT:
                    printf( "FLOAT    " ) ;
                    break ;
                case MN_STRING:
                    printf( "STRING   " ) ;
                    break ;
                default:
                    printf( "         " ) ;
                    break ;
            }

            printf( "%-20s", mnemonics[n].name ) ;
            if ( i == MN_SYSCALL || i == MN_SYSPROC )
            {
                printf( "%-8s (%d)", sysproc_name( param ), param ) ;
            }
            else if ( i == MN_SENTENCE )
            {
#ifdef __BGDRTM__
                if ( dcb.sourcecount[param >> 24] )
                    printf( "%s:%d -> %s", dcb.sourcefiles[param >> 24], param & 0xFFFFFF, dcb.sourcelines[param >> 24] [( param & 0xFFFFFF )-1] ) ;
#else
                printf( "%s:%d", files[param>>24], param&(( 1 << 24 ) - 1 ) ) ;
#endif
            }
            else if ( i == ( MN_PUSH | MN_FLOAT ) )
            {
                printf( "%-8f", *(( float * )&param ) ) ;
            }
            else
            {
                switch ( MN_PARAMS( i ) )
                {
                    case    1:
                        printf( "%-8d", param ) ;
                        break;
                }
            }
            printf( "\n" ) ;
        }
        n++ ;
    }
}
