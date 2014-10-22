#include "imagecache.h"

ImageCache::ImageCache() {
    maxCacheSize = 1024 * 1024 * 50; // 50 MB compressed data;
}

Image* ImageCache::findImagePointer(Image* image)
{
    for (int i = 0; i < cachedImages.size(); i++)
        if (cachedImages.at(i)->compare(image->getInfo()))
            return cachedImages.at(i);
    return NULL;
}

void ImageCache::pushImage(Image* image)
{
    float imageMBytes = (float) image->getSize() / 1024;
    while (cacheSize() > maxCacheSize - imageMBytes && cacheSize() != 0)
    {
        delete cachedImages.last();
        cachedImages.remove(cacheSize() - 1);
    }
    
    cachedImages.push_back(image);
}

ImageCache::~ImageCache()
{

}

qint64 ImageCache::cacheSize() const
{
    qint64 size = 0;
    for (int i = 0; i < cachedImages.size(); i++)
        size += cachedImages.at(i)->getSize();
    return size;
}