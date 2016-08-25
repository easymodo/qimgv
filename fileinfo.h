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

enum fileType { NONE, STATIC, ANIMATED, VIDEO };

class FileInfo : public QObject
{
public:
    FileInfo(QString path, QObject *parent = 0);
    ~FileInfo();
    
    QString directoryPath();
    QString filePath();
    QString fileName();

    // in KB
    int fileSize();
    fileType getType();
   // const char* extension();

    // file extension (guessed from mime-type)
    const char* fileExtension();

private:
    QFileInfo fileInfo;
    QDateTime lastModified;
    fileType type;
    const char* extension;

    void setFile(QString path);

    // guesses file type from its contents
    // and sets extension
    void guessType();
};

#endif // FILEINFO_H
