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

    bool insert(QString name, std::shared_ptr<Image> img);
    void trimTo(QStringList list);

    std::shared_ptr<Image> get(QString name);
    bool release(QString name);
    bool reserve(QString name);
    const QList<QString> keys();

private:
    QMap<QString, CacheItem*> items;
};

#endif // CACHE_H
