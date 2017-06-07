#ifndef THUMBNAILER_H
#define THUMBNAILER_H

#include <QRunnable>
#include <QThread>
#include <sourceContainers/thumbnail.h>
#include "cache.h"
#include <imagefactory.h>

class Thumbnailer : public QObject, public QRunnable
{
    Q_OBJECT
public:
    Thumbnailer(ImageCache* _cache, QString _path, int _target, int _size, bool _squared);
    ~Thumbnailer();

    void run();
    QString path;
    int target, size;
    bool squared;

private:
    ImageCache* cache;
    ImageFactory *factory;
signals:
    void thumbnailReady(int, Thumbnail*);
};

#endif // THUMBNAILER_H
