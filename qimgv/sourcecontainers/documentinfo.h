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

#ifdef USE_EXIV2

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
    QMimeType mimeType() const;

    // file extension (guessed from mime-type)
    const char *format() const;
    int exifOrientation() const;

    QDateTime lastModified() const;
    void refresh();
    void loadExifInfo();
    QMap<QString, QString> getExifTags();

private:
    QFileInfo fileInfo;
    DocumentType mDocumentType;
    int mOrientation;
    QString mFormat;

    // guesses file type from its contents
    // and sets extension
    void detectFormat();
    void loadExifOrientation();
    bool detectAPNG();
    bool detectAnimatedWebP();
    QMap<QString, QString> exifTags;
    QMimeType mMimeType;
};

#endif // DOCUMENTINFO_H
