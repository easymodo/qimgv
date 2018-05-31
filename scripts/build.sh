#!/bin/bash

DIR=build

cd ..
#just in case
rm -r ${DIR}
mkdir ${DIR}

cd build
echo "Cleaning up build directory..."
make clean

# All options are off by default.
# Currently available are: VIDEO_SUPPORT, KDE_BLUR
# Example - blur support, no video support:
# cmake -DKDE_BLUR=ON -DCMAKE_INSTALL_PREFIX=/usr/ -DCMAKE_BINARY_DIR=${DIR}/ ..
cmake -DVIDEO_SUPPORT=ON -DCMAKE_INSTALL_PREFIX=/usr/ -DCMAKE_BINARY_DIR=${DIR}/ ..

make -j4

cd ../scripts

echo "build finished."
#./install.sh
