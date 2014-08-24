#ifndef FILEINFO_H
#define FILEINFO_H

#include <QString>
#include <QSize>
#include <QDebug>
#include <QFileInfo>

enum fileType { NONE, STATIC, GIF };

class FileInfo
{
public:
    double aspect;
    int fileNumber; // -1 = default (no file open)
    FileInfo();
    FileInfo(QString *_path);
    fileType setFile(QString path);
    void setDimensions(QSize _size);
    void clear();
    QFileInfo qInfo;
    QSize size;
    fileType type;
    QString getInfo();

private:

};

#endif // FILEINFO_H
