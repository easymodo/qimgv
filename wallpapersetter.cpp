#include "wallpapersetter.h"

WallpaperSetter::WallpaperSetter() {

}

void WallpaperSetter::setWallpaper(QString path) {
    switch(probeOS()) {
        case 2: setWallpaperWin(path); break;
    }
}

void WallpaperSetter::setWallpaperWin(QString path) {
    bool ok = SystemParametersInfo(SPI_SETDESKWALLPAPER,
                                   0,
                                   (PVOID)path.utf16(),
                                   SPIF_UPDATEINIFILE | SPIF_SENDWININICHANGE);
    qDebug() <<"wallpaper changed:"<<(ok ? "true" : "false");
}
