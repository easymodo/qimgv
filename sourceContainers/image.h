#ifndef IMAGE_H
#define IMAGE_H

#include "../lib/imagelib.h"
#include "../lib/stuff.h"
#include "../imageinfo.h"
#include "../settings.h"
#include <QObject>
#include <QString>
#include <QIODevice>
#include <QDebug>
#include <QPixmap>
#include <QPixmapCache>
#include <QThread>
#include <QMutex>

class Image {
public:
    Image();
    virtual ~Image() = 0;
    virtual QPixmap* getPixmap() = 0;
    virtual const QImage* getImage() = 0;
    ImageType type();
    virtual void load() = 0;
    QString getPath();
    virtual int height() = 0;
    virtual int width() = 0;
    virtual QSize size() = 0;
    bool isLoaded();
    ImageInfo* info();

    virtual void crop(QRect newRect) = 0;
    virtual void rotate(int grad) = 0;

    virtual void lock();
    virtual void unlock();
    ImageInfo* imageInfo;

    virtual void save() = 0;
    virtual void save(QString destinationPath) = 0;

protected:
    bool loaded;
    QString path;
    QSize resolution;
    QMutex mutex;
};

#endif // IMAGE_H
