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

AM_MAINTAINER_MODE

echo "You Machine: ${target_cpu} ${target_os}"

AC_ARG_WITH(bennu-prefix,[  --with-bennu-prefix=PFX Prefix where Bennu is installed (optional)],
            bennu_prefix="$withval", bennu_prefix="/opt/bennu")

case "$target_os" in
    linux-gnu*)
        CFLAGS+=" -Wall"
        LDFLAGS+=" -lc -ldl"
	    ;;

    mingw32)
	    CFLAGS+=" -DWIN32 -D_WIN32"
	    ;;

esac

CFLAGS+=" -I${bennu_prefix}/include -I/usr/local/include -I/usr/include "
LDFLAGS+=" -L/usr/local/lib -L/usr/lib"

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h])

# Checks for typedefs, structures, and compiler characteristics.

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
