#pragma once

#include <QSemaphore>
#include "sourcecontainers/image.h"

class CacheItem {
public:
    CacheItem();
    CacheItem(std::shared_ptr<Image> _contents);
    ~CacheItem();

    std::shared_ptr<Image> getContents();

    void lock();
    void unlock();

    int lockStatus();
private:
    std::shared_ptr<Image> contents;
    QSemaphore *sem;
};

