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
#include <QThread>

class Image : public QObject
{
    Q_OBJECT
public:
    Image(FileInfo*);
    ~Image();

    QImage* getImage();
    QMovie* getMovie();
    int getType();
    int ramSize();
    FileInfo* getInfo() const;
    // size in kbytes
    qint64 getSize();
    QString getName();
    QString getResolution();
    QDateTime getModifyDate();
    void loadImage();
    QString getPath();
    float getAspect();
    int height();
    int width();
    QSize size();
    bool compare(Image*);
    bool isInUse();
    void setInUse(bool);

private:
    QImage* image;
    QMovie* movie;
    FileInfo* info;
    QSize resolution;
    float aspectRatio;
    bool inUseFlag;
};

#endif // QIMAGELOADER_H
