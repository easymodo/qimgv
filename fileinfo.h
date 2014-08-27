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
    double aspect;
    FileInfo();
    FileInfo(QString *_path);
    ~FileInfo();

    fileType setFile(QString path);
    void setDimensions(QSize _size);
    void clear();
    QString getPath();
    QString getName();
    QString getInfo();
    QDateTime getLastModified();
    fileType getType();
    QSize getSize();

private:
    QFileInfo qInfo;
    QDateTime lastModified;
    fileType type;
    QSize size;
};

#endif // FILEINFO_H
