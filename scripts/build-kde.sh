#!/bin/bash

DIR=build

cd ..
#just in case
rm -r ${DIR}
mkdir ${DIR}

cd build
echo "Cleaning up build directory..."
make clean

# Available options: 
# VIDEO_SUPPORT (default: ON)
# KDE_BLUR      (default: OFF)
# Example - build with blur & video support:
# cmake -DKDE_BLUR=ON -DCMAKE_INSTALL_PREFIX=/usr/ -DCMAKE_BINARY_DIR=${DIR}/ ..

cmake -DVIDEO_SUPPORT=ON -DKDE_BLUR=ON -DCMAKE_INSTALL_PREFIX=/usr/ -DCMAKE_BINARY_DIR=${DIR}/ ..

make -j4

cd ../scripts

echo "build finished."
#./install.sh
