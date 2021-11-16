#!/bin/sh
#
# Copyright � 2006-2013 SplinterGU (Fenix/Bennugd)
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

cd 3rdparty/des-4.04b
case $1 in
    release)
        make -f Makefile.uni clean
        ;;
esac
make -f Makefile.uni
cd -

COMMON_LDFLAGS="-L$PWD/3rdparty/des-4.04b -ldes"
COMMON_CFLAGS="-I$PWD/3rdparty/des-4.04b -DUSE_LIBDES"

cd core
./make-fakedl.sh

case $1 in
    release)
        ./configure --enable-static COMMON_LDFLAGS="$COMMON_LDFLAGS" COMMON_CFLAGS="$COMMON_CFLAGS" CFLAGS="$COMMON_CFLAGS" && make clean
        cd bgdrtm && make clean
        cd -
        ;;
esac

cd bgdrtm
make
cd -

cd ../modules
case $1 in
    release)
        ./configure --enable-static COMMON_LDFLAGS="$COMMON_LDFLAGS" COMMON_CFLAGS="$COMMON_CFLAGS" CFLAGS="$COMMON_CFLAGS" && make clean
        ;;
esac
make
cd ..

### sdl-config hack for libjoy

EXTRA_STATIC_CFLAGS="$(cflags) $(sdl-config --cflags)"
EXTRA_STATIC_LDFLAGS=$(ldflags)

export EXTRA_STATIC_CFLAGS
export EXTRA_STATIC_LDFLAGS

cd core/bgdc
case $1 in
    release)
        make clean
        ;;
esac
make
cd -

cd core/bgdi
case $1 in
    release)
        make clean
        ;;
esac
make
cd -

echo "### Copying files to bin folder ###"

mkdir -p bin/$TARGET 2>/dev/null
#cp 3rdparty/des-4.04b/libdes.so bin/$TARGET
cp core/bgdi/src/bgdi bin/$TARGET
cp core/bgdc/src/bgdc bin/$TARGET

echo "### Build done! ###"

exit 0
