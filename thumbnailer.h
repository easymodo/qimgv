#ifndef THUMBNAILER_H
#define THUMBNAILER_H

#include <QRunnable>
#include <QThread>
#include <sourceContainers/thumbnail.h>
#include "imagecache.h"
#include <imagefactory.h>

class Thumbnailer : public QObject, public QRunnable
{
    Q_OBJECT
public:
    Thumbnailer(ImageCache* _cache, QString _path, int _target);

    void run();
    QString path;
    int target;
private:
    ImageCache* cache;
signals:
    void thumbnailReady(int, Thumbnail*);
};

#endif // THUMBNAILER_H
