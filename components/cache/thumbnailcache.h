#ifndef THUMBNAILCACHE_H
#define THUMBNAILCACHE_H

#include <QObject>
#include <QDir>
#include <QMutex>
#include <QDebug>
#include "settings.h"
#include "sourcecontainers/thumbnail.h"

class ThumbnailCache : public QObject
{
    Q_OBJECT
public:
    explicit ThumbnailCache();

    void saveThumbnail(QPixmap *image, QString hash);
    QPixmap* readThumbnail(QString hash);
    QString thumbnailPath(QString hash);
    bool exists(QString hash);

signals:

public slots:

private:
    // we are still bottlenecked by disk access anyway
    QMutex mutex;
    QString cacheDirPath;
};

#endif // THUMBNAILCACHE_H
