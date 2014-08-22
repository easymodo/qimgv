#ifndef FILEINFO_H
#define FILEINFO_H

#include <QString>
#include <QSize>
#include <QDebug>
#include <QFileInfo>

enum fileType { NONE, STATIC, GIF };

class fileInfo
{
public:
    double aspectRatio;
    int fileNumber; // -1 = default (no file open)
    fileInfo();
    fileInfo(QString *_path);
    bool setFile(QString *_path);
    void setDimensions(QSize _size);
    void clear();
    QFileInfo qInfo;
    QSize size;
    fileType type;
    QString getInfo();

private:

};

#endif // FILEINFO_H
