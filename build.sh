#!/bin/bash
#rm -rf ./build
#mkdir build && cd build
make clean
sudo find . -name 'moc_*' -exec rm -rf {} \;
cmake . -DCMAKE_INSTALL_PREFIX=/usr/
make -j4
#chmod +x ./qimgv
#cp ./qimgv ../qimgv
#echo "##################################################################";
#echo "NOTE:";
#echo "make sure to put path to ffmpeg/avconv executable in preferences";
#echo "/usr/bin/ffmpeg";
#echo "or";
#echo "/usr/bin/avconv";
#echo "In ubuntu avconv is provided by libav-tools package."
#make clean
#cd ..
Now run install.sh as root
