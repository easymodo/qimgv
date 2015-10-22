#include "wallpapersetter.h"

WallpaperSetter::WallpaperSetter() {

}

void WallpaperSetter::setWallpaper(QString path) {
#ifdef _WIN32
    bool ok = SystemParametersInfo(SPI_SETDESKWALLPAPER,
                                   0,
                                   (PVOID) path.utf16(),
                                   SPIF_UPDATEINIFILE | SPIF_SENDWININICHANGE);
    qDebug() << "wallpaper changed:" << (ok ? "true" : "false");
#elif defined __linux__
    QString command = "/usr/bin/hsetroot -center " + path;
    qDebug() << "Setting wallaper with hsetroot:";
    qDebug() << command;
    QProcess process;
    process.setProcessChannelMode(QProcess::ForwardedChannels);
    process.start(command);
    process.waitForFinished();
    process.close();

    qDebug() << "In case that didnt work your wallpaper is at ~/.wallpaper.jpg";
#endif
}
