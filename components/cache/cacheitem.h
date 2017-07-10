#ifndef CACHEITEM_H
#define CACHEITEM_H

#include <QSemaphore>
#include "sourcecontainers/image.h"

class CacheItem {
public:
    CacheItem();
    CacheItem(Image *_contents);
    ~CacheItem();

    Image *getContents();

    void lock();
    void unlock();

    int lockStatus();
private:
    Image *contents;
    QSemaphore *sem;
};

#endif // CACHEITEM_H
