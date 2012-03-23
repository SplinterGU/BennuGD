#!/bin/sh

WIN32CROSS=/usr/i686-w64-mingw32

## HOST and TARGET - These should be the canonical tool names of your tool.
## For the sake of this script HOST and TARGET should be the same.
## Defaults would be 'i686-w64-mingw32' for a normal WIN32CROSS tool-chain.

HOST=i686-w64-mingw32
TARGET=i686-w64-mingw32
BUILD=`uname -m`
PKG_CONFIG_PATH=/usr/i686-w64-mingw32/sys-root/mingw/lib/pkgconfig

## -- END WIN32CROSS USER SETTINGS

export WIN32CROSS
export HOST
export TARGET
export PKG_CONFIG_PATH

PREFIX=$WIN32CROSS
export PREFIX

PATH=$PATH:$WIN32CROSS/bin
export PATH

# Do not edit below here
CC="${HOST}-gcc"
CXX="${HOST}-g++"
AR="${HOST}-ar"
STRIP="${HOST}-strip"
RANLIB="${HOST}-ranlib"

CFLAGS="-O2 -I${WIN32CROSS}/include -I${WIN32CROSS}/include/libxml2 -I${WIN32CROSS}/include/SDL"
LDFLAGS="-L${WIN32CROSS}/lib"
#PKG_CONFIG="${WIN32CROSS}/bin/pkg-config"

export CC
export CXX
export AR
export STRIP
export RANLIB
export CFLAGS
export LDFLAGS
export PKG_CONFIG

echo Current settings.
echo
echo Install root/Working dir	= $WIN32CROSS
echo Tool locations 		    = $WIN32CROSS/bin
echo Host/Target		        = $HOST / $TARGET
echo

echo CC         = $CC
echo CXX        = $CXX
echo AR         = $AR
echo STRIP      = $STRIP
echo RANLIB     = $RANLIB

echo CFLAGS     = $CFLAGS
echo LDFLAGS    = $LDFLAGS
echo PKG_CONFIG = $PKG_CONFIG

echo "### Building 3rd party software ###"
cd 3rdparty/des-4.04b
case $1 in
    release)
        make clean -e TARGET=$TARGET -e CC=$CC && make gcc -e TARGET=$TARGET -e CC=$CC
        ;;

    *)
        make
        ;;
esac
if [ $? -ne 0 ]; then
    echo "*** ABORT ***"
    exit 1
fi
cd -

echo "### Building BennuGD Core ###"

cd core
case $1 in
    release)
        ./configure --prefix=${PREFIX} --target=${TARGET} --host=${HOST} --build=${BUILD} --enable-shared PKG_CONFIG_LIBDIR=${PKG_CONFIG_PATH} && make clean && make
        ;;

    *)
        make
        ;;
esac
if [ $? -ne 0 ]; then
    echo "*** ABORT ***"
    exit 1
fi
cd -

echo "### Building BennuGD Modules ###"

cd modules
case $1 in
    release)
        ./configure --prefix=${PREFIX} --target=${TARGET} --host=${HOST} --build=${BUILD} --enable-shared PKG_CONFIG_LIBDIR=${PKG_CONFIG_PATH} && make clean && make
        ;;

    *)
        make
        ;;
esac
if [ $? -ne 0 ]; then
    echo "*** ABORT ***"
    exit 1
fi
cd -

echo "### Building BennuGD Tools ###"

cd tools/moddesc
case $1 in
    release)
        ./configure --prefix=${PREFIX} --target=${TARGET} --host=${HOST} --build=${BUILD} --enable-shared PKG_CONFIG_LIBDIR=${PKG_CONFIG_PATH} && make clean && make
        ;;

    *)
        make
        ;;
esac
if [ $? -ne 0 ]; then
    echo "*** ABORT ***"
    exit 1
fi
cd -

echo "### Copying files to bin folder ###"

mkdir -p bin/$TARGET 2>/dev/null
cp 3rdparty/des-4.04b/libdes.dll bin/$TARGET
cp core/bgdi/src/.libs/bgdi.exe bin/$TARGET
cp core/bgdc/src/.libs/bgdc.exe bin/$TARGET
cp core/bgdrtm/src/.libs/libbgdrtm.dll bin/$TARGET
cp modules/mod*/.libs/mod*.dll bin/$TARGET
cp modules/lib*/.libs/lib*.dll bin/$TARGET
cp tools/moddesc/.libs/moddesc.exe bin/$TARGET

strip bin/$TARGET/*

echo "### Build done! ###"

exit 0
