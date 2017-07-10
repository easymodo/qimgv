#include "cacheitem.h"

CacheItem::CacheItem() {
    contents = nullptr;
    sem = new QSemaphore(1);
}

CacheItem::CacheItem(Image *_contents) {
    contents = _contents;
    sem = new QSemaphore(1);
}

CacheItem::~CacheItem() {
    if(contents)
        delete contents;
    delete sem;
}

Image *CacheItem::getContents() {
    return contents;
}

void CacheItem::lock() {
    sem->acquire(1);
}

void CacheItem::unlock() {
    sem->release(1);
}

int CacheItem::lockStatus() {
    return sem->available();
}
