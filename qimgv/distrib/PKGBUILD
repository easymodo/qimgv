# This is an example PKGBUILD file. Use this as a start to creating your own,
# and remove these comments. For more information, see 'man PKGBUILD'.
# NOTE: Please fill out the license field for your package! If it is unknown,
# then please put 'unknown'.

# Maintainer: easymodo <easymodofrf@gmail.com>
pkgname=qimgv-git
_pkgname=qimgv
pkgver=v0.43.r13.g27d8602
pkgrel=1
#epoch=
pkgdesc="Simple image viewer written in qt. Supports webm playback."
arch=('i686' 'x86_64')
url="http://github.com/easymodo/qimgv"
license=('GPL3')
#groups=()
depends=('qt5-base' 'qt5-imageformats' 'qt5-multimedia' 'qt5-svg')
makedepends=('git' 'cmake' 'qt5-tools')
#checkdepends=()
optdepends=('gstreamer0.10-base-plugins: webm playback'
			'ffmpeg: thumbnails for webm')
provides=("qimgv")
conflicts=("qimgv")
#replaces=()
#backup=()
#options=()
#install=$pkgname.install
#changelog=
source=('git+https://github.com/easymodo/qimgv.git')
#noextract=()
md5sums=('SKIP')

pkgver() {
	cd "$_pkgname"
	git describe --long --tags | sed 's/\([^-]*-g\)/r\1/;s/-/./g'
}

prepare() {
	echo "waiting"
	read
	cd "$_pkgname"
	rm -rf ./build
	mkdir build
	cd build
	cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr -DCMAKE_BUILD_TYPE=Release ..
}

package() {
	cd "$_pkgname/build"
	make
	mv ./qimgv ../
	make DESTDIR="$pkgdir/usr" install
	cd ..
	mkdir -p "$pkgdir/usr/share/applications"
	mkdir -p "$pkgdir/usr/share/icons"
	cp -r "./distrib/hicolor" "$pkgdir/usr/share/icons"
	cp -r "./distrib/qimgv.desktop" "$pkgdir/usr/share/applications"	
}
