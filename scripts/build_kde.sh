#!/bin/bash

DIR=build

cd ..
#just in case
rm -r ${DIR}
mkdir ${DIR}

cd build
echo "Cleaning up build directory..."
make clean

cmake -DCMAKE_INSTALL_PREFIX=/usr/ -DCMAKE_BINARY_DIR=${DIR}/ -DWITH_KDE_BLUR=ON ..
make -j4
cd ../scripts
echo "build finished."
#./install.sh
