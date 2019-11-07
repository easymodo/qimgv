#pragma once

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
