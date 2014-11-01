#include "imagecache.h"

ImageCache::ImageCache() {
    applySettings();
    connect(globalSettings, SIGNAL(settingsChanged()),
            this, SLOT(applySettings()));
}

Image* ImageCache::findImage(Image* image)
{
    lock();
    for (int i = 0; i < cachedImages.size(); i++)
        if (cachedImages.at(i)->compare(image)) {
            Image* tmp = cachedImages.at(i);
            unlock();
            return tmp;
        }
    unlock();
    return NULL;
}

bool ImageCache::isFull() {
    lock();
    if(cacheSize()>maxCacheSize) {
        unlock();
        return true;
    }
    else {
        unlock();
        return false;
    }
}

bool ImageCache::cacheImage(Image* image) {
    return pushImage(image, false);
}

bool ImageCache::cacheImageForced(Image* image) {
    return pushImage(image, true);
}

bool ImageCache::pushImage(Image* image, bool forced) {
    lock();
    float imageMBytes = (float) image->ramSize();
    shrinkTo(maxCacheSize - imageMBytes);
    if(forced || (!forced && cacheSize() <= maxCacheSize - imageMBytes ||
                  cachedImages.count() == 0)) {
        cachedImages.push_front(image);
        unlock();
        return true;
    }
    else {
        unlock();
        return false;
    }
}

void ImageCache::readSettings() {
    lock();
    maxCacheSize = globalSettings->s.value("cacheSize").toInt();
    if(maxCacheSize < 32) {
        maxCacheSize = 32;
        globalSettings->s.setValue("cacheSize","32");
    }
    unlock();
}

void ImageCache::applySettings() {
    readSettings();
    shrinkTo(maxCacheSize);
}

// delete images until cache size is less than MB
void ImageCache::shrinkTo(int MB) {
    //while (cacheSize() > MB && cachedImages.length() > 1) // wipes previous
   while (cacheSize() > MB && cachedImages.length() > 2) // leaves previous
    {
        if(!cachedImages.first()->isInUse()) {
            qDebug() << "CACHE: deleting " << cachedImages.first()->getName();
            delete cachedImages.first();
            cachedImages.removeFirst();
        }
        else {
            break;
        }
    }

    //while (cacheSize() > MB && cachedImages.length() > 1) // wipes previous
    while (cacheSize() > MB && cachedImages.length() > 2) // leaves previous
    {
        if(!cachedImages.last()->isInUse()) {
            qDebug() << "CACHE: deleting " << cachedImages.last()->getName();
            delete cachedImages.last();
            cachedImages.removeLast();
        }
        else {
            break;
        }
    }
}

void ImageCache::lock() {
    mutex.lock();
}

void ImageCache::unlock() {
    mutex.unlock();
}

ImageCache::~ImageCache()
{

}

// NOT thread-safe
qint64 ImageCache::cacheSize() const
{
    qint64 size = 0;
    for (int i = 0; i < cachedImages.size(); i++)
        size += cachedImages.at(i)->ramSize();
    return size;
}
