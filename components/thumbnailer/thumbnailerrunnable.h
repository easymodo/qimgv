#ifndef THUMBNAILERRUNNABLE_H
#define THUMBNAILERRUNNABLE_H

#include <QRunnable>
#include <QProcess>
#include <QThread>
#include <QCryptographicHash>
#include <ctime>
#include "sourcecontainers/thumbnail.h"
#include "components/cache/thumbnailcache.h"
#include "utils/imagefactory.h"
#include "settings.h"

#include <QImageWriter>

class ThumbnailerRunnable : public QObject, public QRunnable
{
    Q_OBJECT
public:
    ThumbnailerRunnable(ThumbnailCache* _cache, QString _path, int _size, bool _squared);
    ~ThumbnailerRunnable();
    void run();

private:
    QString generateIdString();
    QImage* createScaledThumbnail(ImageInfo *img, int size, bool squared);
    QString path;
    int size;
    bool squared;
    ThumbnailCache* thumbnailCache;
    QSize originalSize;

signals:
    void taskStart(QString);
    void taskEnd(Thumbnail*, QString);
};

#endif // THUMBNAILERRUNNABLE_H
