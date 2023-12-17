#!/bin/bash
# This builds a complete qimgv-x64 package. Result is placed in qimgv/qimgv-x64_<version>
# Warning: Some stuff will be left over behind after building (C:/qt and C:/opencv-4.5.5-minimal)

#CFL='-ffunction-sections -fdata-sections -march=native -mtune=native -O3 -pipe'
CFL='-ffunction-sections -fdata-sections -O3 -pipe'
LDFL='-Wl,--gc-sections'

if [[ -z "$RUNNER_TEMP" ]]; then
  MSYS_DIR="C:/msys64/mingw64"
else
  MSYS_DIR="$(cd "$RUNNER_TEMP" && pwd)/msys64/mingw64"
fi
CUSTOM_QT_DIR="C:/qt/5.15.3-mingw64-slim"
OPENCV_DIR="C:/opencv-minimal-4.5.5"
SCRIPTS_DIR=$(dirname $(readlink -f $0)) # this file's location (/path/to/qimgv/scripts)
SRC_DIR=$(dirname $SCRIPTS_DIR)
BUILD_DIR=$SRC_DIR/build
EXT_DIR=$SRC_DIR/_external
rm -rf "$EXT_DIR"
mkdir "$EXT_DIR"
MPV_DIR=$EXT_DIR/mpv

# ------------------------------------------------------------------------------
echo "PREPARING BUILD DIR"
rm -rf $BUILD_DIR
mkdir -p $BUILD_DIR

# ------------------------------------------------------------------------------
echo "UPDATING DEPENDENCY LIST"
wget -O $BUILD_DIR/msys2-build-deps.txt https://raw.githubusercontent.com/easymodo/qimgv-deps-bin/main/msys2-build-deps.txt
wget -O $BUILD_DIR/msys2-dll-deps.txt https://raw.githubusercontent.com/easymodo/qimgv-deps-bin/main/msys2-dll-deps.txt

# ------------------------------------------------------------------------------
echo "INSTALLING MSYS2 BUILD DEPS"
MSYS_DEPS=$(cat $BUILD_DIR/msys2-build-deps.txt | sed 's/\n/ /')
pacman -S $MSYS_DEPS --noconfirm --needed

# ------------------------------------------------------------------------------
echo "GETTING Qt"
mkdir C:/qt
cd C:/qt
wget -O 5.15.3-mingw64-slim.7z https://github.com/easymodo/qt-builds/releases/download/5.15.3-mingw64-slim/5.15.3-mingw64-slim.7z
7z x 5.15.3-mingw64-slim.7z -y
rm 5.15.3-mingw64-slim.7z

# ------------------------------------------------------------------------------
echo "GETTING OpenCV"
mkdir $OPENCV_DIR
cd $OPENCV_DIR
wget -O opencv-minimal-4.5.5-x64.7z https://github.com/easymodo/qimgv-deps-bin/releases/download/x64/opencv-minimal-4.5.5-x64.7z
7z x opencv-minimal-4.5.5-x64.7z -y
rm opencv-minimal-4.5.5-x64.7z

# ------------------------------------------------------------------------------
echo "GETTING MPV"
mkdir $MPV_DIR
cd $MPV_DIR
wget -O mpv-x64.7z https://github.com/easymodo/qimgv-deps-bin/releases/download/x64/mpv-x86_64-20230402-git-0f13c38.7z
7z x mpv-x64.7z -y
rm mpv-x64.7z

# ------------------------------------------------------------------------------
# We are using prebuilt opencv but feel free to compile instead
#echo "BUILDING OPENCV"
#cd $EXT_DIR
#git clone --depth 1 --branch 4.5.5 https://github.com/opencv/opencv.git
#cd opencv
#rm -rf ./build
#cmake -S ./ -B build -G Ninja \
#    -DCMAKE_INSTALL_PREFIX="$OPENCV_DIR" \
#    -DCMAKE_BUILD_TYPE=Release \
#    -DBUILD_LIST='core,imgproc' \
#    -DWITH_1394=OFF \
#    -DWITH_VTK=OFF \
#    -DWITH_FFMPEG=OFF \
#    -DWITH_GSTREAMER=OFF \
#    -DWITH_DSHOW=OFF \
#    -DWITH_QUIRC=OFF \
#    -DBUILD_SHARED_LIBS=ON \
#    -DCMAKE_C_FLAGS="$CFL" -DCMAKE_CXX_FLAGS="$CFL" -DCMAKE_EXE_LINKER_FLAGS="$LDFL"
#ninja install -C build

# ------------------------------------------------------------------------------
echo "BUILDING"
#rm -rf $BUILD_DIR
sed -i 's|opencv4/||' $SRC_DIR/qimgv/3rdparty/QtOpenCV/cvmatandqimage.{h,cpp}
cmake -S $SRC_DIR -B $BUILD_DIR -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_PREFIX_PATH=$CUSTOM_QT_DIR \
    -DOpenCV_DIR=$OPENCV_DIR \
    -DOPENCV_SUPPORT=ON \
    -DVIDEO_SUPPORT=ON \
    -DMPV_DIR=$MPV_DIR \
    -DCMAKE_CXX_FLAGS="$CFL" -DCMAKE_EXE_LINKER_FLAGS="$LDFL"
ninja -C $BUILD_DIR

# ------------------------------------------------------------------------------
echo "BUILDING IMAGEFORMATS"
# We are using libjxl from MSYS2 but feel free to compile instead
#cd $EXT_DIR
#git clone --depth 1 https://github.com/libjxl/libjxl.git --recursive
#cd libjxl
#rm -rf build
#cmake -S . -B build -G "Ninja" \
#    -DCMAKE_INSTALL_PREFIX=$MSYS_DIR \
#    -DCMAKE_BUILD_TYPE=Release -DJPEGXL_ENABLE_PLUGINS=OFF \
#    -DBUILD_TESTING=OFF -DJPEGXL_WARNINGS_AS_ERRORS=OFF \
#    -DJPEGXL_ENABLE_SJPEG=OFF -DJPEGXL_ENABLE_BENCHMARK=OFF \
#    -DJPEGXL_ENABLE_EXAMPLES=OFF -DJPEGXL_ENABLE_MANPAGES=OFF \
#    -DJPEGXL_FORCE_SYSTEM_BROTLI=ON
#ninja install -C build

# qt-jpegxl-image-plugin
cd $EXT_DIR
git clone --depth 1 https://github.com/novomesk/qt-jpegxl-image-plugin.git
cd qt-jpegxl-image-plugin
rm -rf build
cmake -S . -B build -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DQT_MAJOR_VERSION=5 \
    -DCMAKE_PREFIX_PATH=$CUSTOM_QT_DIR
ninja -C build

# qt-avif-image-plugin
cd $EXT_DIR
git clone https://github.com/novomesk/qt-avif-image-plugin
cd qt-avif-image-plugin
rm -rf build
cmake -S . -B build -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DQT_MAJOR_VERSION=5 \
    -DCMAKE_PREFIX_PATH=$CUSTOM_QT_DIR
ninja -C build

# QtApng
cd $EXT_DIR
git clone https://github.com/Skycoder42/QtApng.git
cd QtApng
rm -rf build
mkdir build && cd build
$CUSTOM_QT_DIR/bin/qmake.exe ..
mingw32-make -j4

# qt-heif-image-plugin
cd $EXT_DIR
git clone https://github.com/jakar/qt-heif-image-plugin.git
cd qt-heif-image-plugin
rm -rf build
cmake -S . -B build -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_PREFIX_PATH=$CUSTOM_QT_DIR
ninja -C build

# qtraw
cd $EXT_DIR
git clone https://gitlab.com/mardy/qtraw
cd qtraw
rm -rf build
mkdir build && cd build
$CUSTOM_QT_DIR/bin/qmake.exe .. DEFINES+="LIBRAW_WIN32_CALLS=1"
mingw32-make -j4

# ------------------------------------------------------------------------------
echo "PACKAGING"
# 0 - prepare dir
cd $SRC_DIR
BUILD_NAME=qimgv-x64_$(git describe --tags)
PACKAGE_DIR=$SRC_DIR/$BUILD_NAME
rm -rf $PACKAGE_DIR
mkdir $PACKAGE_DIR

# 1 - copy qimgv build
cp $BUILD_DIR/qimgv/qimgv.exe $PACKAGE_DIR
mkdir $PACKAGE_DIR/plugins
cp $BUILD_DIR/plugins/player_mpv/player_mpv.dll $PACKAGE_DIR/plugins
cp -r $BUILD_DIR/qimgv/translations/ $PACKAGE_DIR/

# 2 - copy qt dlls
cd $CUSTOM_QT_DIR/bin
cp Qt5Core.dll Qt5Gui.dll Qt5PrintSupport.dll Qt5Svg.dll Qt5Widgets.dll $PACKAGE_DIR
cd $CUSTOM_QT_DIR/plugins
cp -r iconengines imageformats printsupport styles $PACKAGE_DIR
mkdir $PACKAGE_DIR/platforms
cp platforms/qwindows.dll $PACKAGE_DIR/platforms

# 3 - copy msys dlls
MSYS_DLLS=$(cat $BUILD_DIR/msys2-dll-deps.txt | sed 's/\n/ /')
cd $MSYS_DIR/bin
cp $MSYS_DLLS $PACKAGE_DIR

# 4 - copy imageformats
cp $EXT_DIR/qt-jpegxl-image-plugin/build/bin/imageformats/libqjpegxl5.dll $PACKAGE_DIR/imageformats
cp $EXT_DIR/qt-avif-image-plugin/build/bin/imageformats/libqavif5.dll $PACKAGE_DIR/imageformats
cp $EXT_DIR/QtApng/build/plugins/imageformats/qapng.dll $PACKAGE_DIR/imageformats
cp $EXT_DIR/qt-heif-image-plugin/build/bin/imageformats/libqheif.dll $PACKAGE_DIR/imageformats
cp $EXT_DIR/qtraw/build/src/imageformats/qtraw.dll $PACKAGE_DIR/imageformats

# 5 - copy opencv & mpv
cd $OPENCV_DIR/x64/mingw/bin
cp libopencv_core455.dll libopencv_imgproc455.dll $PACKAGE_DIR
cd $MPV_DIR/bin/x86_64
cp mpv.exe libmpv-2.dll $PACKAGE_DIR

# 6 - misc
mkdir $PACKAGE_DIR/cache
mkdir $PACKAGE_DIR/conf
mkdir $PACKAGE_DIR/thumbnails
cp -r $SRC_DIR/qimgv/distrib/mimedata/data $PACKAGE_DIR

cd $SRC_DIR
echo "PACKAGING DONE"
