#include "imagecache.h"

ImageCache::ImageCache() {

}


Image* getCurrent() {
    return current;
}

Image* getPrev() {
    return prev;
}

Image* getNext() {
    return next;
}

ImageCache::~ImageCache() {
    delete current, next, prev;
}
