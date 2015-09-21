#ifndef FILEINFO_H
#define FILEINFO_H

#include <QString>
#include <QSize>
#include <QDebug>
#include <QFileInfo>
#include <QDateTime>
#include <cmath>

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

private:
    void setFile(QString path);
    QFileInfo fileInfo;
    QDateTime lastModified;
    fileType type;
    fileType guessType();
};

#endif // FILEINFO_H
