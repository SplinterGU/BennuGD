#!/bin/sh

PKG_CONFIG_PATH=/usr/lib/i386-linux-gnu/pkgconfig

#TARGET=gnu-linux
TARGET=i386-linux-gnu

echo "### Building BennuGD Core ###"

cd core
case $1 in
    release)
        ./configure --build=i686-pc-linux-gnu PKG_CONFIG_PATH=${PKG_CONFIG_PATH} PKG_CONFIG_LIBDIR=${PKG_CONFIG_PATH} && make clean && make
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
        ./configure --build=i686-pc-linux-gnu PKG_CONFIG_PATH=${PKG_CONFIG_PATH} PKG_CONFIG_LIBDIR=${PKG_CONFIG_PATH} SDL_CFLAGS="-I/usr/include/SDL -D_GNU_SOURCE=1 -D_REENTRANT" SDL_LIBS="-L/usr/lib/${TARGET} -lSDL-1.2" && make clean && make
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
        ./configure --build=i686-pc-linux-gnu PKG_CONFIG_PATH=${PKG_CONFIG_PATH} PKG_CONFIG_LIBDIR=${PKG_CONFIG_PATH} && make clean && make
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
#cp 3rdparty/des-4.04b/libdes.so bin/$TARGET
cp core/bgdi/src/.libs/bgdi bin/$TARGET
cp core/bgdc/src/bgdc bin/$TARGET
cp core/bgdrtm/src/.libs/libbgdrtm.so bin/$TARGET
cp $(find modules -name '*.so') bin/$TARGET
cp tools/moddesc/moddesc bin/$TARGET

echo "### Build done! ###"

exit 0
