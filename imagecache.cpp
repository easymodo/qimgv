#include "imagecache.h"

ImageCache::ImageCache() {

}

Image* ImageCache::getCurrent() {
    return current;
}

Image* ImageCache::getPrev() {
    return prev;
}

Image* ImageCache::getNext() {
    return next;
}

ImageCache::~ImageCache() {
    delete current, next, prev;
}
