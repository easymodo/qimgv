#ifndef FILEINFO_H
#define FILEINFO_H

#include <QString>
#include <QSize>
#include <QDebug>
#include <QFileInfo>
#include <QDateTime>

enum fileType { NONE, STATIC, GIF };

class FileInfo
{
public:
    FileInfo(QString *path);
    ~FileInfo();
    bool inUse;
    QString getDirPath();
    QString getFilePath();
    QString getName();
    QDateTime getLastModified();
    fileType getType();

private:
    void setFile(QString path);
    QFileInfo fInfo;
    QDateTime lastModified;
    fileType type;
};

#endif // FILEINFO_H
