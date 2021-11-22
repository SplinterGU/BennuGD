#!/bin/sh
#
# Copyright © 2006-2013 SplinterGU (Fenix/Bennugd)
#
# This file is part of Bennu - Game Development
#
# This software is provided 'as-is', without any express or implied
# warranty. In no event will the authors be held liable for any damages
# arising from the use of this software.
#
# Permission is granted to anyone to use this software for any purpose,
# including commercial applications, and to alter it and redistribute it
# freely, subject to the following restrictions:
#
#    1. The origin of this software must not be misrepresented; you must not
#    claim that you wrote the original software. If you use this software
#    in a product, an acknowledgment in the product documentation would be
#    appreciated but is not required.
#
#    2. Altered source versions must be plainly marked as such, and must not be
#    misrepresented as being the original software.
#
#    3. This notice may not be removed or altered from any source
#    distribution.
#


TARGET=i386-linux-gnu-static

cflags()
{
for i in $(ls -d $PWD/modules/*|grep -v mathi); do if [ -d $i ]; then echo -n " -I$i "; fi; done
}

ldflags()
{
for i in $(find modules -name '*.a'|grep -v mod_mathi); do echo -n "-L$PWD/$(dirname $i) -l$(basename $i .a) "|sed 's/-llib/-l/g'; done
}

echo "#### building necessaries files ####"

cd 3rdparty/des-4.04b > /dev/null 2> /dev/null
case $1 in
    release)
        make -f Makefile.uni clean
        ;;
esac
make -f Makefile.uni
cd - > /dev/null 2> /dev/null

COMMON_LDFLAGS="-L$PWD/3rdparty/des-4.04b -ldes"
COMMON_CFLAGS="-I$PWD/3rdparty/des-4.04b -DUSE_LIBDES"

EXTRA_STATIC_CFLAGS="$(sdl-config --cflags)"
export EXTRA_STATIC_CFLAGS

EXTRA_STATIC_LDFLAGS=$(ldflags)
export EXTRA_STATIC_LDFLAGS

cd core > /dev/null 2> /dev/null
./make-fakedl.sh

case $1 in
    release)
        ./configure --enable-static COMMON_LDFLAGS="$COMMON_LDFLAGS" COMMON_CFLAGS="$COMMON_CFLAGS" CFLAGS="$COMMON_CFLAGS" && make clean
        make clean
        ;;
esac

cd bgdrtm > /dev/null 2> /dev/null
make
if [ $? -ne 0 ]; then
    echo "*** ABORT ***"
    exit 1
fi
cd - > /dev/null 2> /dev/null

cd ../modules > /dev/null 2> /dev/null
case $1 in
    release)
        ./configure --enable-static COMMON_LDFLAGS="$COMMON_LDFLAGS" COMMON_CFLAGS="$COMMON_CFLAGS" CFLAGS="$COMMON_CFLAGS" && make clean
        ;;
esac
make
if [ $? -ne 0 ]; then
    echo "*** ABORT ***"
    exit 1
fi
cd .. > /dev/null 2> /dev/null

EXTRA_STATIC_CFLAGS="$(cflags) $(sdl-config --cflags)"

rm -Rf core/bgdi/src/bgdi core/bgdc/src/bgdc

cd core/bgdc > /dev/null 2> /dev/null
case $1 in
    release)
        make clean
        ;;
esac
make
if [ $? -ne 0 ]; then
    echo "*** ABORT ***"
    exit 1
fi
cd - > /dev/null 2> /dev/null

cd core/bgdi > /dev/null 2> /dev/null
case $1 in
    release)
        make clean
        ;;
esac
make
if [ $? -ne 0 ]; then
    echo "*** ABORT ***"
    exit 1
fi
cd - > /dev/null 2> /dev/null

echo "### Copying files to bin folder ###"

mkdir -p bin/$TARGET 2>/dev/null
#cp 3rdparty/des-4.04b/libdes.so bin/$TARGET
cp core/bgdi/src/bgdi bin/$TARGET
cp core/bgdc/src/bgdc bin/$TARGET

echo "### Build done! ###"

exit 0
