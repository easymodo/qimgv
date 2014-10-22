#ifndef IMAGECACHE_H
#define IMAGECACHE_H

#include "image.h"
#include <QVector>

class ImageCache
{
public:
    ImageCache();
    ~ImageCache();
    Image* findImagePointer(Image* image);
    void pushImage(Image* image);
    qint64 cacheSize() const;
private:
    QVector<Image*> cachedImages;
    uint maxCacheSize;
};

#endif // IMAGECACHE_H
