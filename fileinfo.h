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
    int fileNumber;
    fileInfo();
    fileInfo(QString *_path);
    fileInfo(QString *_path, QSize _size);
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
