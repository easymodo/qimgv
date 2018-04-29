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

private:
    QFileInfo fileInfo;
    QDateTime lastModified;
    DocumentType mImageType;
    const char* mExtension;

    // guesses file type from its contents
    // and sets extension
    void detectType();
};

#endif // IMAGEINFO_H
