#include "cacheitem.h"

CacheItem::CacheItem() {
    sem = new QSemaphore(1);
}

CacheItem::CacheItem(std::shared_ptr<Image> _contents) {
    contents = _contents;
    sem = new QSemaphore(1);
}

CacheItem::~CacheItem() {
    delete sem;
}

std::shared_ptr<Image> CacheItem::getContents() {
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
