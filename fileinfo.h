#ifndef FILEINFO_H
#define FILEINFO_H

#include <QString>
#include <QSize>
#include <QUrl>
#include <QMimeDatabase>
#include <QDebug>
#include <QFileInfo>
#include <QDateTime>
#include <cmath>
#include "lib/stuff.h"

enum fileType { NONE, STATIC, GIF, VIDEO };

class FileInfo : public QObject
{
public:
    FileInfo();
    FileInfo(QString path, QObject *parent = 0);
    ~FileInfo();
    
    /* size in MB*/
    float getFileSize();

    QString getDirectoryPath();
    QString getFilePath();
    QString getFileName();
    fileType getType();
    const char* getExtension();

private:
    void setFile(QString path);
    QFileInfo fileInfo;
    QDateTime lastModified;
    void guessType();
    fileType type;
    const char* extension;
};

#endif // FILEINFO_H
