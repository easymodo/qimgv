#!/bin/bash

#just in case
mkdir build

cd build

cmake -DCMAKE_INSTALL_PREFIX=/usr/ ..
make -j4
echo "Running sudo make install"
sudo make install

echo "Cleaning up build directory..."
make clean
cd ..
echo "Done."
echo "Installation finished! Enjoy."
