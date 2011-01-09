#!/bin/sh

## -- DINGUX USER SETTINGS

## DINGUX - This should point to the root of your tool-chain {i.e. folder above the BIN dir}

DINGUX=~/devel/dingux-a320/mipsel-linux-uclibc/usr

## HOST and TARGET - These should be the canonical tool names of your tool.
## For the sake of this script HOST and TARGET should be the same.
## Defaults would be 'mipsel-linux'.

HOST=mipsel-linux
TARGET=mipsel-linux
BUILD=`uname -m`
PKG_CONFIG_PATH=~/devel/dingux-a320/mipsel-linux-uclibc/usr/lib/pkgconfig

## -- END DINGUX USER SETTINGS

export DINGUX
export HOST
export TARGET
export PKG_CONFIG_PATH

PREFIX=$DINGUX
export PREFIX

PATH=$PATH:$DINGUX/bin
export PATH

ln -s `whereis -b pkg-config | sed 's/pkg-config\: //g'` ~/devel/dingux-a320/mipsel-linux-uclibc/usr/bin/pkg-config

# Do not edit below here
CC="${DINGUX}/bin/${HOST}-gcc"
CXX="${DINGUX}/bin/${HOST}-g++"
AR="${DINGUX}/bin/${HOST}-ar"
STRIP="${DINGUX}/bin/${HOST}-strip"
RANLIB="${DINGUX}/bin/${HOST}-ranlib"
LD="${DINGUX}/bin/${HOST}-gcc"

# -msoft-float -funroll-loops -ffast-math -fomit-frame-pointer -fno-strength-reduce -finline-functions -G0 -march=mips32 -mtune=r4600 -mno-mips16
CFLAGS="-DTARGET_DINGUX_A320 -O2 -fstrength-reduce -fthread-jumps -fexpensive-optimizations -fomit-frame-pointer -frename-registers -pipe -G0 -DMPU_JZ4740 -ffast-math -msoft-float -I${DINGUX}/include"
LDFLAGS="-L${DINGUX}/lib"
PKG_CONFIG="${DINGUX}/bin/pkg-config"

export CC
export CXX
export AR
export STRIP
export RANLIB
export LD

export CFLAGS
export LDFLAGS
export PKG_CONFIG

echo Current settings.
echo
echo Install root/Working dir	= $DINGUX
echo Tool locations 		    = $DINGUX/bin
echo Host/Target		        = $HOST / $TARGET
echo

echo CC         = $CC
echo CXX        = $CXX
echo AR         = $AR
echo STRIP      = $STRIP
echo RANLIB     = $RANLIB
echo LD         = $LD

echo CFLAGS     = $CFLAGS
echo LDFLAGS    = $LDFLAGS
echo PKG_CONFIG = $PKG_CONFIG

echo "### Building 3rd party software ###"
cd 3rdparty/des-4.04b
make clean
make
cd -

echo "### Building BennuGD Core ###"

cd core
./configure --prefix=${PREFIX} --target=${TARGET} --host=${HOST} --build=${BUILD} --enable-shared
make clean
make
cd -

echo "### Building BennuGD Modules ###"

cd modules
./configure --prefix=${PREFIX} --target=${TARGET} --host=${HOST} --build=${BUILD} --enable-shared
make clean
make
cd -

#echo "### Building BennuGD Tools ###"
#
#cd tools/moddesc
#./configure --prefix=${PREFIX} --target=${TARGET} --host=${HOST} --build=${BUILD} --enable-shared
#make clean
#make
#cd -

echo "### Copying files to bin folder ###"

mkdir bin 2>/dev/null
cp 3rdparty/des-4.04b/libdes.so bin/
cp core/bgdi/src/.libs/bgdi bin/
cp core/bgdc/src/bgdc bin/
cp core/bgdrtm/src/.libs/libbgdrtm.so bin/
cp modules/mod*/.libs/mod*.so bin
cp modules/lib*/.libs/lib*.so bin/

echo "### Build done! ###"
