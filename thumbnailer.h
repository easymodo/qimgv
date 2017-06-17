#ifndef THUMBNAILER_H
#define THUMBNAILER_H

#include <QRunnable>
#include <QThread>
#include <sourceContainers/thumbnail.h>
#include "thumbnailcache.h"
#include "cache.h"
#include <imagefactory.h>
#include <ctime>
#include "settings.h"
#include <malloc.h>

class Thumbnailer : public QObject, public QRunnable
{
    Q_OBJECT
public:
    Thumbnailer(ThumbnailCache* _cache, QString _path, int _target, int _size, bool _squared);
    ~Thumbnailer();

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

#endif // THUMBNAILER_H
