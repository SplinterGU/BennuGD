#!/bin/sh

TARGET=gnu-linux

#echo "### Building 3rd party software ###"
#cd 3rdparty/des-4.04b
#make clean
#make
#cd -

echo "### Building BennuGD Core ###"

cd core
./configure && make clean && make
if [ $? -ne 0 ]; then
    echo "*** ABORT ***"
    exit 1
fi
cd -

echo "### Building BennuGD Modules ###"

cd modules
./configure && make clean && make
if [ $? -ne 0 ]; then
    echo "*** ABORT ***"
    exit 1
fi
cd -

echo "### Building BennuGD Tools ###"

cd tools/moddesc
./configure && make clean && make
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
cp modules/mod*/.libs/mod*.so bin/$TARGET
cp modules/lib*/.libs/lib*.so bin/$TARGET
cp tools/moddesc/moddesc bin/$TARGET

echo "### Build done! ###"

exit 0
