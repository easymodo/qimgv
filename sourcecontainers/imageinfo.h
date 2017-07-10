#ifndef IMAGEINFO_H
#define IMAGEINFO_H

#include <QString>
#include <QSize>
#include <QUrl>
#include <QMimeDatabase>
#include <QDebug>
#include <QFileInfo>
#include <QDateTime>
#include <cmath>
#include "utils/stuff.h"

enum ImageType { NONE, STATIC, ANIMATED, VIDEO };

class ImageInfo {
public:
    ImageInfo(QString path);
    ~ImageInfo();
    
    QString directoryPath();
    QString filePath();
    QString fileName();
    QString baseName();

    // in KB
    int fileSize();
    ImageType imageType();

    // file extension (guessed from mime-type)
    const char* extension();

private:
    QFileInfo fileInfo;
    QDateTime lastModified;
    ImageType mImageType;
    const char* mExtension;

    // guesses file type from its contents
    // and sets extension
    void detectType();
};

#endif // IMAGEINFO_H
