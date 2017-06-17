#ifndef THUMBNAILERRUNNABLE_H
#define THUMBNAILERRUNNABLE_H

#include <QRunnable>
#include <QThread>
#include <QCryptographicHash>
#include <malloc.h>
#include <ctime>
#include "sourcecontainers/thumbnail.h"
#include "components/cache/thumbnailcache.h"
#include "utils/imagefactory.h"
#include "settings.h"

class ThumbnailerRunnable : public QObject, public QRunnable
{
    Q_OBJECT
public:
    ThumbnailerRunnable(ThumbnailCache* _cache, QString _path, int _target, int _size, bool _squared);
    ~ThumbnailerRunnable();

    void run();
    QString path;
    int target, size;
    bool squared;

private:
    ThumbnailCache* thumbnailCache;

    QString generateIdString();
    QImage* createThumbnailImage(ImageInfo *img, int size, bool squared);
    QImage* videoThumbnailStub();
signals:
    void thumbnailReady(int, Thumbnail*);
};

#endif // THUMBNAILERRUNNABLE_H
