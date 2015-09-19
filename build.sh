#!/bin/bash
rm -rf ./build
mkdir build && cd build
cmake ../ -DCMAKE_INSTALL_PREFIX=/usr/
make -j3
chmod +x ./qimgv
cp ./qimgv ../qimgv
echo "##################################################################";
echo "NOTE:";
echo "make sure to put path to ffmpeg/avconv executable in preferences";
echo "/usr/bin/ffmpeg";
echo "or";
echo "/usr/bin/avconv";
echo "In ubuntu avconv is provided by libav-tools package."
make clean
cd ..

