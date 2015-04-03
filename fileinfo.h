#ifndef FILEINFO_H
#define FILEINFO_H

#include <QString>
#include <QSize>
#include <QDebug>
#include <QFileInfo>
#include <QDateTime>
#include <cmath>

enum fileType { NONE, STATIC, GIF };

class FileInfo
{
public:
    FileInfo();
    FileInfo(QString *path);
    ~FileInfo();
    bool inUse;
    QString getDirectoryPath();
    QString getFilePath();
    QString getFileName();
    QDateTime getLastModifiedDate();
    fileType getType();
    
    /* size in MB*/
    float getFileSize();

    void setType(fileType _type);

    const char* getExtension() const;
    int getHeight() const;
    void setHeight(int value);
    int getWidth() const;
    void setWidth(int value);

private:
    void setFile(QString path);
    QFileInfo fileInfo;
    QDateTime lastModified;
    fileType type;
    int positions[2];
    int width, height;
    const char* extension;
    void detectType();
};

#endif // FILEINFO_H
