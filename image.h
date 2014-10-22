#ifndef QIMAGELOADER_H
#define QIMAGELOADER_H

#include <fileinfo.h>
#include <QString>
#include <QIODevice>
#include <QDebug>
#include <QMovie>
#include <QPixmap>
#include <QPixmapCache>
#include <QImage>
#include <QObject>

class Image : public QObject
{
    Q_OBJECT
public:
    Image();
    Image(FileInfo*);
    Image(QString path);
    ~Image();
    
    QImage* getImage() const;
    QMovie* getMovie() const;
    int getType() const;
    FileInfo* getInfo() const;
    // size in kbytes
    qint64 getSize() const;
    QString getName() const;
    QString getResolution() const;
    QDateTime getModifyDate() const;
    void loadImage(QString path);
    QString getPath() const;
    float getAspect();
    int height();
    int width();
    QSize size();

    bool compare(FileInfo*);

private:
    QString mPath;
    QImage* image;
    QMovie* movie;
    FileInfo *info;
    QSize resolution;
    float aspectRatio;
};

#endif // QIMAGELOADER_H
