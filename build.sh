#!/bin/bash

#just in case
mkdir build

cd build
echo "Cleaning up build directory..."
make clean

cmake -DCMAKE_INSTALL_PREFIX=/usr/ ..
make -j4
cd ..
echo "build finished."
./install.sh
