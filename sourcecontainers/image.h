#ifndef IMAGE_H
#define IMAGE_H

#include <QObject>
#include <QString>
#include <QIODevice>
#include <QDebug>
#include <QPixmap>
#include <QPixmapCache>
#include <QThread>
#include <QSemaphore>
#include "utils/imagelib.h"
#include "utils/stuff.h"
#include "sourcecontainers/imageinfo.h"

class Image {
public:
    Image();
    virtual ~Image() = 0;
    virtual QPixmap* getPixmap() = 0;
    virtual const QImage* getImage() = 0;
    ImageType type();
    QString getPath();
    virtual int height() = 0;
    virtual int width() = 0;
    virtual QSize size() = 0;
    bool isLoaded();
    ImageInfo* info();

    ImageInfo* imageInfo;

    virtual void save() = 0;
    virtual void save(QString destPath) = 0;

    QString name();

protected:
    virtual void load() = 0;
    bool loaded;
    QString path;
    QSize resolution;
};

#endif // IMAGE_H
