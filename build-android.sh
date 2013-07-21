#!/bin/sh

## -- ANDROID USER SETTINGS

## ANDROID - This should point to the root of your tool-chain {i.e. folder above the BIN dir}

BASEDIR=/opt/android-ndk-r7b/toolchains/arm-linux-androideabi-4.4.3/prebuilt/linux-x86/arm-linux-androideabi
PLATFORM=/opt/android-ndk-r7b/platforms/android-8/arch-arm

## HOST and TARGET - These should be the canonical tool names of your tool.
## For the sake of this script HOST and TARGET should be the same.
## Defaults would be 'arm-linux-androideabi' for a normal android tool-chain.

HOST=arm-linux-androideabi
TARGET=arm-linux-androideabi
BUILD=`uname -m`
PKG_CONFIG_PATH=${BASEDIR}/lib/pkgconfig

## -- END ANDROID USER SETTINGS

export BASEDIR
export HOST
export TARGET
export PKG_CONFIG_PATH

PREFIX=$BASEDIR
export PREFIX

PATH=$BASEDIR/bin:$PATH
export PATH

# Do not edit below here
CC="${BASEDIR}/../bin/${HOST}-gcc -nostdlib"
CXX="${BASEDIR}/../bin/${HOST}-g++"
AR="${BASEDIR}/../bin/${HOST}-ar"
STRIP="${BASEDIR}/../bin/${HOST}-strip"
RANLIB="${BASEDIR}/../bin/${HOST}-ranlib"

CFLAGS="-DTARGET_ANDROID -DANDROID -O3 -DARM -D_ARM_ASSEM_ -I${BASEDIR}/include -I${BASEDIR}/include/libxml2 -I${BASEDIR}/include/SDL -I${PLATFORM}/usr/include"
LDFLAGS="-nostdlib --shared -L${BASEDIR}/lib -L${PLATFORM}/usr/lib"
#PKG_CONFIG="${BASEDIR}/bin/pkg-config"


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
echo Install root/Working dir	= $BASEDIR
echo Tool locations 		    = $BASEDIR/bin
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
cd 3rdparty/SDL-1.2.15
case $1 in
    release)
        make -f Makefile.minimal clean && make -f Makefile.minimal
        ;;

    *)
        make -f Makefile.minimal
        ;;
esac
if [ $? -ne 0 ]; then
    echo "*** ABORT ***"
    exit 1
fi
cd -

cd 3rdparty/dietlibc-glob
case $1 in
    release)
        make clean && make
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

cd 3rdparty/des-4.04b
case $1 in
    release)
        make clean && make
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
        LIBTOOLFLAGS=-nostdlib ZLIB_CFLAGS=-I${PLATFORM}/include ZLIB_LIBS="-L${PLATFORM}/usr/lib -lz" ./configure --prefix=${PREFIX} --target=${TARGET} --host=${HOST} --build=${BUILD} --enable-shared PKG_CONFIG_LIBDIR=${PKG_CONFIG_PATH} && make clean && make
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
cp 3rdparty/des-4.04b/libdes.so bin/$TARGET
cp core/bgdi/src/.libs/bgdi bin/$TARGET
cp core/bgdc/src/bgdc bin/$TARGET
cp core/bgdrtm/src/.libs/libbgdrtm.so bin/$TARGET
cp $(find modules -name '*.so') bin/$TARGET
cp tools/moddesc/moddesc bin/$TARGET

echo "### Build done! ###"

exit 0
