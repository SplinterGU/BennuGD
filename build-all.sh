#!/bin/sh

rm -Rf bin/*

#./build-android.sh
#if [ $? -ne 0 ]; then
#    echo "*** ABORT ***"
#    exit 1
#fi

./build-caanoo.sh release
if [ $? -ne 0 ]; then
    echo "*** ABORT ***"
    exit 1
fi

./build-dingux-a320.sh release
if [ $? -ne 0 ]; then
    echo "*** ABORT ***"
    exit 1
fi

./build-openwiz.sh release
if [ $? -ne 0 ]; then
    echo "*** ABORT ***"
    exit 1
fi

#./build-pandora.sh
#if [ $? -ne 0 ]; then
#    echo "*** ABORT ***"
#    exit 1
#fi

#./build-static.sh
#if [ $? -ne 0 ]; then
#    echo "*** ABORT ***"
#    exit 1
#fi

#./build-win32-cross-opensuse.sh release
#if [ $? -ne 0 ]; then
#    echo "*** ABORT ***"
#    exit 1
#fi

./build-win32-cross.sh
if [ $? -ne 0 ]; then
    echo "*** ABORT ***"
    exit 1
fi

#./build-win32.sh
#if [ $? -ne 0 ]; then
#    echo "*** ABORT ***"
#    exit 1
#fi

./build-linux.sh release
if [ $? -ne 0 ]; then
    echo "*** ABORT ***"
    exit 1
fi

cd installer
./make-installers.sh
cd -
