#ifndef WALLPAPERSETTER_H
#define WALLPAPERSETTER_H

#ifdef _WIN32
#include <windows.h>
#endif
#include <QString>
#include <QProcess>
#include <QDebug>
#include "utils/stuff.h"

class WallpaperSetter
{
public:
    WallpaperSetter();

    static void setWallpaper(QString path);
};

#endif // WALLPAPERSETTER_H
