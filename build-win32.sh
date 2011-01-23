#!/bin/sh

TARGET=gnu-win32

#echo "### Building 3rd party software ###"
#cd 3rdparty/des-4.04b
#make clean
#make
#cd -

echo "### Building BennuGD Core ###"

cd core
./configure
make clean
make
cd -

echo "### Building BennuGD Modules ###"

cd modules
./configure
make clean
make
cd -

echo "### Building BennuGD Tools ###"

cd tools/moddesc
./configure
make clean
make
cd -

echo "### Copying files to bin folder ###"

mkdir -p bin/$TARGET 2>/dev/null
#cp 3rdparty/des-4.04b/libdes.dll bin/$TARGET
cp core/bgdi/src/.libs/bgdi.exe bin/$TARGET
cp core/bgdc/src/.libs/bgdc.exe bin/$TARGET
cp core/bgdrtm/src/.libs/libbgdrtm.dll bin/$TARGET
cp modules/mod*/.libs/mod*.dll bin/$TARGET
cp modules/lib*/.libs/lib*.dll bin/$TARGET
cp tools/moddesc/moddesc.exe bin/$TARGET

strip bin/$TARGET/*

echo "### Build done! ###"
