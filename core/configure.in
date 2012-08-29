AC_INIT([bennu],[1.0.0])

LT_INIT([disable-static])

PKG_PROG_PKG_CONFIG()

AC_CONFIG_MACRO_DIR([m4])

AC_CANONICAL_TARGET
AC_ARG_PROGRAM

AC_LIBTOOL_WIN32_DLL
AC_LIBTOOL_DLOPEN
AC_PROG_LIBTOOL

AM_MAINTAINER_MODE

echo "Your machine: ${target_cpu} ${target_os}"

AC_ARG_ENABLE(debug,    [  --enable-debug       enable use of Debug     (default: no)], enable_debug=$enableval, enable_debug=no)

AC_CHECK_TOOL(WINDRES, windres, )
AM_CONDITIONAL(HAVE_WINDRES, test "$WINDRES" != "")

dnl Aqui es donde se chequeara el SO, pero solo hay una opcion xD
case "$target_os" in
    linux-androideabi)
        COMMON_CFLAGS="$COMMON_CFLAGS -DUSE_LIBDES $CFLAGS -I`pwd`/../3rdparty/des-4.04b -I`pwd`/../3rdparty/dietlibc-glob -Wall -DTARGET_LINUX"
        COMMON_LDFLAGS="$COMMON_LDFLAGS $LDFLAGS -L`pwd`/../3rdparty/dietlibc-glob -L`pwd`/../3rdparty/des-4.04b -ldes -lc -ldl "
        ;;

    linux-gnueabi)
        COMMON_CFLAGS="$COMMON_CFLAGS -DUSE_LIBDES $CFLAGS -I`pwd`/../3rdparty/des-4.04b -Wall -DTARGET_LINUX"
        COMMON_LDFLAGS="$COMMON_LDFLAGS $LDFLAGS -L`pwd`/../3rdparty/des-4.04b -ldes -lc -ldl"
        ;;

    linux-gnu*)
        COMMON_CFLAGS="-Wall -DTARGET_LINUX"
        COMMON_LDFLAGS="$COMMON_LDFLAGS -lc -ldl"

        case "$target_cpu" in
            i*86)
                COMMON_CFLAGS="$COMMON_CFLAGS -m32 -DUSE_OPENSSL"
                COMMON_LDFLAGS="$COMMON_LDFLAGS -m32 -L/lib32"
                LIBSSL="crypto"
                USE_OPENSSL=yes
                ;;

            arm)
                COMMON_CFLAGS="$COMMON_CFLAGS -DUSE_LIBDES $CFLAGS -I`pwd`/../3rdparty/des-4.04b"
                COMMON_LDFLAGS="$COMMON_LDFLAGS $LDFLAGS -L`pwd`/../3rdparty/des-4.04b -ldes"
                ;;

            mipsel)
                COMMON_CFLAGS="$COMMON_CFLAGS -DUSE_LIBDES $CFLAGS -I`pwd`/../3rdparty/des-4.04b"
                COMMON_LDFLAGS="$COMMON_LDFLAGS $LDFLAGS -L`pwd`/../3rdparty/des-4.04b -ldes -lz"
                ;;

        esac
        ;;

    mingw32)
        INCLUDES="-I/usr/local/include"
        COMMON_CFLAGS="$COMMON_CFLAGS -DUSE_LIBDES $CFLAGS -I`pwd`/../3rdparty/des-4.04b"
        COMMON_LDFLAGS="$COMMON_LDFLAGS $LDFLAGS -L`pwd`/../3rdparty/des-4.04b -ldes -L/usr/local/lib -lshlwapi -lkernel32 -Wl,--enable-auto-import"
	    COMMON_CFLAGS="$COMMON_CFLAGS $INCLUDES -DTARGET_WIN32 -DWIN32 -D_WIN32"
#	    LIBSSL="eay32"
        USE_OPENSSL=no
	    ;;

    mingw32msvc)
        INCLUDES="-I/usr/i586-mingw32msvc/include"
	    COMMON_CFLAGS="$INCLUDES -DUSE_LIBDES -DTARGET_WIN32 -DWIN32 -D_WIN32"
        COMMON_LDFLAGS="$COMMON_LDFLAGS $LDFLAGS -L`pwd`/../3rdparty/des-4.04b -ldes -L/usr/i586-mingw32msvc/lib -lshlwapi -lkernel32 -Wl,--enable-auto-import"
        LIBSSL="crypto"
        USE_OPENSSL=no
	    ;;

    dragonfly*)
        INCLUDES="-I/usr/local/include"
        COMMON_CFLAGS="-Wall -DTARGET_BSD -DUSE_OPENSSL"
	    COMMON_LDFLAGS="-L/usr/local/lib -lpthread"
        LIBSSL="crypto"
        USE_OPENSSL=yes
	    ;;

    darwin*)
        INCLUDES="-I/usr/local/include"
        COMMON_CFLAGS="-Wall -DTARGET_MAC -DUSE_OPENSSL"
	    COMMON_LDFLAGS="-L/usr/local/lib"
        LIBSSL="crypto"
        USE_OPENSSL=yes
	    ;;

    haiku)
        INCLUDES="-I/usr/local/include"
        COMMON_CFLAGS="-Wall -DTARGET_BEOS -DUSE_OPENSSL"
	    COMMON_LDFLAGS="-L/usr/local/lib"
        LIBSSL="crypto"
        USE_OPENSSL=yes
        ;;

esac

AM_INIT_AUTOMAKE(1.6)

dnl Check for compilers
AC_PROG_CC

if test "x$enable_debug" = "xyes"; then
    CFLAGS+=" -g $INCLUDES"
else
    CFLAGS+=" -finline-functions -fno-strict-aliasing $INCLUDES"
fi

if test "x$enable_static" = "xyes"; then
    CFLAGS+=" -D__STATIC__ "
    enable_shared=no
fi

dnl librerias para todos

PKG_CHECK_MODULES(ZLIB, [ zlib ])

if test "$USE_OPENSSL" = "yes"; then
AC_CHECK_LIB($LIBSSL, DES_ecb_encrypt, [AC_CHECK_HEADER(openssl/des.h,CRYPT_LDFLAGS=-l$LIBSSL,)], [AC_MSG_ERROR($LIBSSL not found!)])
fi

dnl make these visible to all Makefiles

COMMON_LDFLAGS="$LDFLAGS $COMMON_LDFLAGS"
COMMON_CFLAGS="$CFLAGS $COMMON_CFLAGS"

AC_SUBST(COMMON_CFLAGS)
AC_SUBST(COMMON_LDFLAGS)
AC_SUBST(CRYPT_LDFLAGS)

AC_CONFIG_FILES([
Makefile
bgdrtm/Makefile
bgdrtm/src/Makefile
bgdc/Makefile
bgdc/src/Makefile
bgdi/Makefile
bgdi/src/Makefile
])
AC_OUTPUT

echo
echo Bennu $VERSION
echo
echo Debug Mode ....... : $enable_debug
echo Static build ..... : $enable_static
echo
echo The binary will be installed in $prefix/bin
echo
echo configure complete, now type \'make\' and pray.
echo
