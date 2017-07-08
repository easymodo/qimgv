#ifndef CACHE2_H
#define CACHE2_H

#include <QDebug>
#include <QHash>
#include <QSemaphore>
#include <QMutexLocker>
#include "sourcecontainers/image.h"
#include "components/cache/cacheitem.h"
#include "utils/imagefactory.h"

class Cache2 {
public:
    explicit Cache2();
    bool contains(QString name);
    void remove(QString name);
    void clear();

    bool insert(QString name, Image *img);
    bool insert(QString name, QString path);
    void lock();
    void unlock();
    void trimTo(QStringList list);

    Image* get(QString name);
    void release(QString name);
    void reserve(QString name);
    const QList<QString> keys();
signals:

public slots:

private:
    QSemaphore *sem;
    QHash<QString, CacheItem*> images;
};

#endif // CACHE2_H
