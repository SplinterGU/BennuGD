#!/bin/sh

TARGET=i386-linux-gnu
MY_PKG_CONFIG_PATH=/usr/lib/${TARGET}/pkgconfig

echo "### Building core... ### "
cd core
case $1 in
    release)
        autoreconf --install && PKG_CONFIG_PATH=${MY_PKG_CONFIG_PATH} CFLAGS=-Wl,--rpath=. ./configure --host=${TARGET} && make 
        ;;
    clean)
        make distclean
        make clean
        rm -rf aclocal.m4 autom4te.cache/ compile config.guess config.log config.status config.sub configure depcomp install-sh libtool ltmain.sh m4/ Makefile.in missing Makefile
        find . -name *.in -exec rm {} \;
        ;;
    *)
        make
        ;;
esac
if [ $? -ne 0 ]; then
    echo "*** ERROR building core! ***"
    exit 1
fi
cd - 

echo "### Building modules... ### "
cd modules
case $1 in
    release)
        autoreconf --install && PKG_CONFIG_PATH=${MY_PKG_CONFIG_PATH} CFLAGS=-Wl,--rpath=. ./configure --host=${TARGET} && make 
        ;;
    clean)
        make distclean
        make clean
        rm -rf aclocal.m4 autom4te.cache/ compile config.guess config.log config.status config.sub configure depcomp install-sh libtool ltmain.sh m4/ Makefile.in missing Makefile
        find . -name *.in -exec rm {} \; 
        ;;
    *)
        make
        ;;
esac
if [ $? -ne 0 ]; then
    echo "*** ERROR building modules! ***"
    exit 1
fi
cd - 

echo "### Building tools... ### "
cd tools/moddesc
case $1 in
    release)
        autoreconf --install && PKG_CONFIG_PATH=${MY_PKG_CONFIG_PATH} CFLAGS=-Wl,--rpath=. ./configure --host=${TARGET} && make 
        ;;
    clean)
        make distclean
        make clean
        rm -rf aclocal.m4 autom4te.cache/ compile config.guess config.log config.status config.sub configure depcomp install-sh libtool ltmain.sh m4/ Makefile.in missing Makefile
        find . -name *.in -exec rm {} \;
        ;;
    *)
        make
        ;;
esac
if [ $? -ne 0 ]; then
    echo "*** ERROR building tools! ***"
    exit 1
fi
cd - 

echo "#### Deploying..."
mkdir -p bin/${TARGET} 2>/dev/null
cp core/bgdi/src/.libs/bgdi bin/${TARGET}
cp core/bgdc/src/bgdc bin/${TARGET}
cp core/bgdrtm/src/.libs/libbgdrtm.so bin/${TARGET}
find modules -name '*.so' -exec cp {} bin/${TARGET} \;
cp tools/moddesc/moddesc bin/${TARGET}

echo "#### Striping..."
strip bin/${TARGET}/bgdi
strip bin/${TARGET}/bgdc
strip bin/${TARGET}/libbgdrtm.so
strip bin/${TARGET}/moddesc
find bin/${TARGET} -name '*.so' -exec strip {} \;

echo "### Done! ###"
