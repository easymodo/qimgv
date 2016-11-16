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
    QString command;
    command = "qdbus org.kde.plasmashell /PlasmaShell org.kde.PlasmaShell.evaluateScript \'var allDesktops = desktops(); print (allDesktops); for (i = 0; i < allDesktops.length; i++) { d = allDesktops[i]; d.wallpaperPlugin = \"org.kde.image\"; d.currentConfigGroup = Array(\"Wallpaper\", \"org.kde.image\", \"General\"); d.writeConfig(\"Image\", \"" + path + "\") } \'";
    QProcess process;
    //process.setProcessChannelMode(QProcess::ForwardedChannels);
    process.start("sh", QStringList() << "-c" << command);
    process.waitForFinished();
    process.close();

    qDebug() << "In case that didnt work your cropped wallpaper is saved at:" << path;
#endif
}
