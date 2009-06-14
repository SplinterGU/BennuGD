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
#ifndef __MESSAGES_H
#define __MESSAGES_H

#define MSG_LIBRARY_NOT_FOUND                   "Library \"%s\" not found"
#define MSG_FILE_NOT_FOUND                      "%s: file not found"
#define MSG_FILE_TOO_BIG                        "%s: file too big"
#define MSG_FILE_EMPTY                          "%s: file is empty"
#define MSG_READ_ERROR                          "%s: file reading error"
#define MSG_DIRECTORY_MISSING                   "You must specify a directory"
#define MSG_TOO_MANY_FILES                      "Too many files specified"
#define MSG_USING                               "Use: %s [options] filename\n\n"
#define MSG_OPTION_D                            "   -d              Debugging mode\n"
#define MSG_OPTIONS                             \
    "   -o ouputfile    Ouput DCB file\n" \
    "   -i dir          Adds the directory to the PATH\n" \
    "   -a              Automaticaly adds all files to the DCB\n" \
    "   -f file         Adds a single file to the DCB\n" \
    "   -l lang         Specify locale settings\n" \
    "   -s stub         Generate a stubbed executable from the given stub\n" \
    "   -g              Stores debugging information at the DCB\n" \
    "   -c              File uses the MS-DOS character set\n" \
    "   -D macro=text   Set a macro\n" \
    "   -p|--pedantic   Don't use automatic declare\n" \
    "   -C options      Specify compiler options\n" \
    "                   Where options are:\n" \
    "                   a    Enable automatic declare functions\n" \
    "\n"
#define MSG_LICENSE                             \
    "This program is free software. You can distribute and/or modify it\n" \
    "under the terms of the GNU General Public License as published by the\n" \
    "Free Software Foundation; either the version 2 of the license or\n" \
    "(at your will) any later version.\n\n" \
    "Read the file COPYING for details\n\n"
#define MSG_COMPILE_ERROR                       "%s%s:%d: error: %s "
#define MSG_COMPILE_WARNING                     "%s%s:%d: warning: %s "
#define MSG_CONSTANT_NAME_EXP                   "Constant name expected"
#define MSG_INVALID_IDENTIFIER                  "Invalid identifier"
#define MSG_EXPECTED                            "\"%s\" expected"
#define MSG_PROCNAME_EXP                        "Procedure name expected"
#define MSG_INVALID_PARAM                       "Parameter name invalid"
#define MSG_INVALID_PARAMT                      "Parameter type invalid"
#define MSG_TOO_MANY_PARAMS                     "Too many parameters in a definition"
#define MSG_INCORRECT_PARAMC                    "Incorrect number of parameters. Function: %s MinParams: %d."
#define MSG_NO_BEGIN                            "BEGIN expected"
#define MSG_NO_END                              "END expected"
#define MSG_ELSE_WOUT_IF                        "ELSE without IF"
#define MSG_PROGRAM_NAME_EXP                    "Program name expected"
#define MSG_PROCESS_NAME_EXP                    "Procedure name expected"
#define MSG_INVALID_TYPE                        "Invalid data type"
#define MSG_UNEXPECTED_TOKEN                    "Unexpected token (too many ENDs?)"
#define MSG_UNEXPECTED_TOKEN_GENERIC            "Unexpected token"
#define MSG_NO_MAIN                             "Main procedure was not defined"
#define MSG_INTEGER_REQUIRED                    "Integer type required"
#define MSG_NUMBER_REQUIRED                     "Numeric type required"
#define MSG_INCOMP_TYPES                        "Data types not compatible with operation"
#define MSG_INCOMP_TYPE                         "Data type not accepted here"
#define MSG_UNKNOWN_IDENTIFIER                  "Unknown identifier"
#define MSG_NOT_AN_ARRAY                        "Not an array or struct array"
#define MSG_BOUND                               "Index out of range"
#define MSG_IDENTIFIER_EXP                      "Identifier expected"
#define MSG_NOT_AN_LVALUE                       "Can't get the address of an inmediate value"
#define MSG_NOT_A_POINTER                       "Pointer required"
#define MSG_VARIABLE_REQUIRED                   "Variable required"
#define MSG_STRUCT_REQUIRED                     "Struct required"
#define MSG_DIVIDE_BY_ZERO                      "Division by zero"
#define MSG_TYPES_NOT_THE_SAME                  "Values are of incompatible type"
#define MSG_CONSTANT_EXP                        "Constant value expected"
#define MSG_STRING_EXP                          "String expected"
#define MSG_NO_LOOP                             "Out of loop"
#define MSG_INVALID_STEP                        "Invalid STEP"
#define MSG_INVALID_SENTENCE                    "Invalid sentence"
#define MSG_VTA                                 "Can't create an array of undefined multiple sizes"
#define MSG_TOO_MANY_AL                         "Too many array levels"
#define MSG_CONSTANT_REDECLARED_AS_VARIABLE     "Constant redeclared as variable"
#define MSG_VARIABLE_REDECLARED_AS_CONSTANT     "Variable redeclared as constant"
#define MSG_VARIABLE_REDECLARE                  "Variable redeclared"
#define MSG_VARIABLE_REDECLARE_DIFF             "Variable redeclared with different type"
#define MSG_IDENTIFIER_EXP                      "Identifier expected"
#define MSG_CANT_INIT_STR                       "This struct can't be initialized"
#define MSG_TOO_MANY_INIT                       "Too many initializers"
#define MSG_TOO_MANY_INCLUDES                   "Too many nested includes"
#define MSG_IDENTIFIER_TOO_LONG                 "Identifier too long"
#define MSG_INVALID_CHAR                        "Invalid Character"
#define MSG_TOO_MANY_TYPES                      "Too many user-defined types"
#define MSG_UNDEFINED_PROC                      "Undefined procedure"
#define MSG_NO_COMPATIBLE_DLL                   "The library is not BennuGD compatible"
#define MSG_TOO_MANY_SYSPROCS                   "Too many system functions"
#define MSG_INCORRECT_PTR_INIT                  "A pointer can only be initialized to 0"
#define MSG_NOT_ENOUGH_INIT                     "Not enough initializers"
#define MSG_MULTIPLE_PROCS_FOUND                "Various conflicting versions of %s found"
#define MSG_QUESTION_INC                        "Incompatible types at the sides of ? operator"
#define MSG_UNKNOWN_PREP                        "Unknown preprocessor directive"
#define MSG_PTR_CONVERSION                      "Invalid conversion of non-pointer to pointer"
#define MSG_CONVERSION                          "Unsupported data type cast"
#define MSG_PROC_ALREADY_DEFINED                "Process already defined"
#define MSG_FRAME_REQUIRES_INT                  "FRAME requires an INT return value type"
#define MSG_VARIABLE_ERROR                      "Variable already defined as process, expected type or variable name"
#define MSG_PROTO_ERROR                         "Conflict with previous declaration"
#define MSG_MACRO_ERROR                         "Macro %s already declared"
#define MSG_OR                                  " or "
#define MSG_TOO_COMPLEX                         "Expression too complex"
#define MSG_EXTRA_CHAR                          "Extra character"
#define MSG_INVALID_EXP                         "Invalid expression"
#define MSG_FILENAME_EXP                        "Filename expected"

#define MSG_ON_PARAM_ERR                        "ERROR/EXIT expected"
#define MSG_GOTO_EXP                            "GOTO/JMP expected"

#endif

