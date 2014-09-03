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
    qint64 getSize();
    void setPositions(int current, int from);
    int getCurrentPos();
    int getMaxPos();

private:
    void setFile(QString path);
    QFileInfo fInfo;
    QDateTime lastModified;
    fileType type;
    int positions[2];
};

#endif // FILEINFO_H
