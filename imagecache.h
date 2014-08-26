#ifndef IMAGECACHE_H
#define IMAGECACHE_H

#include "image.h"

class ImageCache
{
public:
    ImageCache();
    ~ImageCache();
    Image* getCurrent();
    Image* getPrev();
    Image* getNext();
    void setCurrent(Image*);
    void setPrev(Image*);
    void setNext(Image*);
private:
    Image *prev, *current, *next;
};

#endif // IMAGECACHE_H
