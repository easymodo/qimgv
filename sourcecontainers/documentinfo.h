#ifndef DOCUMENTINFO_H
#define DOCUMENTINFO_H

#include <QString>
#include <QSize>
#include <QUrl>
#include <QMimeDatabase>
#include <QDebug>
#include <QFileInfo>
#include <QDateTime>
#include <cmath>
#include <cstring>
#include "utils/stuff.h"

#ifdef WITH_EXIV2

#include <exiv2/exiv2.hpp>
#include <iostream>
#include <iomanip>
#include <cassert>

#endif

#include <QImageReader>

enum DocumentType { NONE, STATIC, ANIMATED, VIDEO };

class DocumentInfo {
public:
    DocumentInfo(QString path);
    ~DocumentInfo();
    
    QString directoryPath() const;
    QString filePath() const;
    QString fileName() const;
    QString baseName() const;
    int fileSize() const;
    DocumentType type() const;

    // file extension (guessed from mime-type)
    const char* extension() const;
    int exifOrientation() const;

    QDateTime lastModified() const;
    void refresh();
    void loadExifInfo();
    QMap<QString, QString> getExifTags();

private:
    QFileInfo fileInfo;
    DocumentType mImageType;
    int mOrientation;
    const char* mExtension;

    // guesses file type from its contents
    // and sets extension
    void detectType();
    void loadExifOrientation();
    bool detectAPNG();
    bool detectAnimatedWebP();
    QMap<QString, QString> exifTags;
};

#endif // DOCUMENTINFO_H
