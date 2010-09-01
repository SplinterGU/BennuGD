#!/bin/sh

## -- OPEN2X USER SETTINGS

## OPEN2X - This should point to the root of your tool-chain {i.e. folder above the BIN dir}

OPEN2X=~/caanoo/GPH_SDK/tools/gcc-4.2.4-glibc-2.7-eabi/arm-gph-linux-gnueabi

## HOST and TARGET - These should be the canonical tool names of your tool.
## For the sake of this script HOST and TARGET should be the same.
## Defaults would be 'arm-open2x-linux' for a normal Open2x tool-chain.

HOST=arm-gph-linux-gnueabi
TARGET=arm-gph-linux-gnueabi
BUILD=`uname -m`
PKG_CONFIG_PATH=~/caanoo/GPH_SDK/tools/gcc-4.2.4-glibc-2.7-eabi/arm-gph-linux-gnueabi/lib/pkgconfig

## -- END OPEN2X USER SETTINGS

export OPEN2X
export HOST
export TARGET
export PKG_CONFIG_PATH

PREFIX=$OPEN2X
export PREFIX

PATH=$OPEN2X/bin:$PATH
export PATH

ln -s `whereis -b pkg-config | sed 's/pkg-config\: //g'` ~/caanoo/GPH_SDK/tools/gcc-4.2.4-glibc-2.7-eabi/arm-gph-linux-gnueabil/bin/pkg-config

# Do not edit below here
CC="${OPEN2X}/../bin/${HOST}-gcc"
CXX="${OPEN2X}/../bin/${HOST}-g++"
AR="${OPEN2X}/../bin/${HOST}-ar"
STRIP="${OPEN2X}/../bin/${HOST}-strip"
RANLIB="${OPEN2X}/../bin/${HOST}-ranlib"

#CFLAGS="-DTARGET_CAANOO -O2 -ffast-math -fomit-frame-pointer -mcpu=arm920t -DARM -D_ARM_ASSEM_ -I${OPEN2X}/include -I${OPEN2X}/include/libxml2 -I${OPEN2X}/include/SDL"

CFLAGS="-DTARGET_CAANOO -mcpu=arm926ej-s -mtune=arm926ej-s -fsigned-char -O3 -msoft-float -fomit-frame-pointer -fstrict-aliasing -mstructure-size-boundary=32 -fexpensive-optimizations -fweb -frename-registers -falign-functions=16 -falign-loops -falign-labels -falign-jumps -finline -finline-functions -fno-common -fno-builtin -fsingle-precision-constant -DARM -D_ARM_ASSEM_ -I${OPEN2X}/include -I${OPEN2X}/include/libxml2 -I${OPEN2X}/include/SDL"
LDFLAGS="-L${OPEN2X}/lib"
PKG_CONFIG="${OPEN2X}/bin/pkg-config"

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
echo Install root/Working dir	= $OPEN2X
echo Tool locations 		= $OPEN2X/bin
echo Host/Target		= $HOST / $TARGET
echo

echo CC         = $CC
echo CXX        = $CXX
echo AR         = $AR
echo STRIP      = $STRIP
echo RANLIB     = $RANLIB

echo CFLAGS     = $CFLAGS
echo LDFLAGS    = $LDFLAGS
echo PKG_CONFIG = $PKG_CONFIG

#cd vendor/des-4.04b; make -f Makefile.openwiz clean all install; cd -
#cd core; ./configure --prefix=${PREFIX} --target=${TARGET} --host=${HOST} --build=${BUILD} --enable-shared ; make -f Makefile; cd -

echo ""
echo "Now do:"
echo "cd project"
echo './configure --prefix=${PREFIX} --target=${TARGET} --host=${HOST} --build=${BUILD} --enable-shared'

