#ifndef CACHE_H
#define CACHE_H

#include <QDebug>
#include <QMap>
#include <QSemaphore>
#include <QMutexLocker>
#include "sourcecontainers/image.h"
#include "components/cache/cacheitem.h"
#include "utils/imagefactory.h"

class Cache {
public:
    explicit Cache();
    bool contains(QString name);
    void remove(QString name);
    void clear();

    bool insert(QString name, Image *img);
    void lock();
    void unlock();
    void trimTo(QStringList list);

    Image* get(QString name);
    bool release(QString name);
    bool reserve(QString name);
    const QList<QString> keys();
signals:

public slots:

private:
    QSemaphore *sem;
    QMap<QString, CacheItem*> images;
};

#endif // CACHE_H
