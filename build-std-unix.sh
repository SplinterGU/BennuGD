#!/bin/sh

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

mkdir bin 2>/dev/null
cp 3rdparty/des-4.04b/libdes.so bin/
cp core/bgdi/src/.libs/bgdi bin/
cp core/bgdc/src/bgdc bin/
cp core/bgdrtm/src/.libs/libbgdrtm.so bin/
cp modules/mod*/.libs/mod*.so bin
cp modules/lib*/.libs/lib*.so bin/
cp tools/moddesc/moddesc bin/

echo "### Build done! ###"
