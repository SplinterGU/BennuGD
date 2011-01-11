#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

#AC_PREREQ(2.56)
AC_INIT(moddesc, 1.0)

AC_CONFIG_SRCDIR([moddesc.c])

AC_CANONICAL_SYSTEM

AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CC
AC_PROG_MAKE_SET

AC_DISABLE_STATIC
AC_LIBTOOL_DLOPEN

AM_PROG_CC_C_O

AM_MAINTAINER_MODE

LT_INIT

AC_CONFIG_MACRO_DIR([m4])

echo "You Machine: ${target_cpu} ${target_os}"

AC_ARG_WITH(bennu-prefix,[  --with-bennu-prefix=PFX Prefix where Bennu is installed (optional)],bennu_prefix="$withval", bennu_prefix="`pwd`/../../core")

case "$target_os" in
    mingw32)
	    CFLAGS+=" -DWIN32 -D_WIN32 "
	    ;;

    *)
        CFLAGS+=" -Wall"
        LDFLAGS+=" -lc -ldl"
	    ;;

esac

COMMON_CFLAGS+="$CFLAGS -I${bennu_prefix}/include"
COMMON_LDFLAGS+="$LDFLAGS"

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h])

AC_SUBST(COMMON_CFLAGS)
AC_SUBST(COMMON_LDFLAGS)

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
