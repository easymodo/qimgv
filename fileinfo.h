#ifndef FILEINFO_H
#define FILEINFO_H

#include <QString>
#include <QSize>
#include <QDebug>
#include <QFileInfo>
#include <QDateTime>
#include <cmath>

class FileInfo
{
public:
    FileInfo();
    FileInfo(QString path);
    ~FileInfo();
    
    /* size in MB*/
    float getFileSize();

    QString getDirectoryPath();
    QString getFilePath();
    QString getFileName();
private:
    void setFile(QString path);
    QFileInfo fileInfo;
    QDateTime lastModified;
};

#endif // FILEINFO_H
