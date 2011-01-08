#!/bin/sh

## -- OPEN2X USER SETTINGS

## OPEN2X - This should point to the root of your tool-chain {i.e. folder above the BIN dir}

OPEN2X=~/devel/wizdev/openwiz/toolchain/arm-openwiz-linux-gnu

## HOST and TARGET - These should be the canonical tool names of your tool.
## For the sake of this script HOST and TARGET should be the same.
## Defaults would be 'arm-open2x-linux' for a normal Open2x tool-chain.

HOST=arm-openwiz-linux-gnu
TARGET=arm-openwiz-linux-gnu
BUILD=`uname -m`
PKG_CONFIG_PATH=~/devel/wizdev/openwiz/toolchain/arm-openwiz-linux-gnu/lib/pkgconfig

## -- END OPEN2X USER SETTINGS

export OPEN2X
export HOST
export TARGET
export PKG_CONFIG_PATH

PREFIX=$OPEN2X
export PREFIX

PATH=$PATH:$OPEN2X/bin
export PATH

ln -s `whereis -b pkg-config | sed 's/pkg-config\: //g'` ~/devel/wizdev/openwiz/toolchain/arm-openwiz-linux-gnu/bin/pkg-config

# Do not edit below here
CC="${OPEN2X}/bin/${HOST}-gcc"
CXX="${OPEN2X}/bin/${HOST}-g++"
AR="${OPEN2X}/bin/${HOST}-ar"
STRIP="${OPEN2X}/bin/${HOST}-strip"
RANLIB="${OPEN2X}/bin/${HOST}-ranlib"

CFLAGS="-DTARGET_GP2X_WIZ -O2 -ffast-math -fomit-frame-pointer -mcpu=arm920t -DARM -D_ARM_ASSEM_ -I${OPEN2X}/include -I${OPEN2X}/include/libxml2 -I${OPEN2X}/include/SDL"
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
echo Tool locations 		    = $OPEN2X/bin
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

echo "### Building BennuGD Tools ###"

cd tools/moddesc
./configure --prefix=${PREFIX} --target=${TARGET} --host=${HOST} --build=${BUILD} --enable-shared
make clean
make
cd -

echo "### Copying files to bin folder ###"

mkdir bin
cp 3rdparty/des-4.0.4b/libdes.so bin/
cp tools/moddesc/moddesc bin/
cp core/bgdi/src/.libs/bgdi bin/
cp core/bgdc/src/bgdc bin/
cp core/bgdrtm/src/.libs/libbgdrtm.so bin/
cp modules/mod*/.libs/mod*.so bin
cp modules/lib*/.libs/lib*.so bin/

echo "### Build done! ###"
