# Maintainer: Eugene G. <easymodofrf@gmail.com>

pkgname=qimgv
_gitname="qimgv"
pkgver=0.29.2.g154f36f
pkgrel=1
pkgdesc="Simple image viewer written in qt."
url=https://github.com/easymodo/qimgv
license=( 'GPL' )
depends=( qt5-base )
arch=( 'i686' 'x86_64' )
makedepends=( git sed )
conflicts=( qimgv )
provides=( qimgv )
source=( git://github.com/easymodo/qimgv.git )
md5sums=('SKIP')

pkgver() {
  cd "$srcdir/$_gitname"
  git describe --tags | sed 's|-|.|g'
}

build() {
    cd "$srcdir/$pkgname"
    cmake -DCMAKE_INSTALL_PREFIX="$pkgdir/usr"
    make
}

package() {
    mkdir -p "$pkgdir/usr/bin/"
    cd "$srcdir/$pkgname"
    install -m755 $pkgname "$pkgdir/usr/bin/"
}