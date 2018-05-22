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

#include <exiv2/exiv2.hpp>

enum DocumentType { NONE, STATIC, ANIMATED, VIDEO };

class DocumentInfo {
public:
    DocumentInfo(QString path);
    ~DocumentInfo();
    
    QString directoryPath() const;
    QString filePath() const;
    QString fileName() const;
    QString baseName() const;

    // in KB
    int fileSize() const;
    DocumentType type() const;

    // file extension (guessed from mime-type)
    const char* extension() const;
    long exifOrientation() const;

private:
    QFileInfo fileInfo;
    QDateTime lastModified;
    DocumentType mImageType;
    long mOrientation;
    const char* mExtension;

    Exiv2::ExifData exifData;
    // guesses file type from its contents
    // and sets extension
    void detectType();
    void loadExif();
};

#endif // IMAGEINFO_H
