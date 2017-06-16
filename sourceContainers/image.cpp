#include "image.h"

Image::Image() : imageInfo(NULL) {
}

Image::~Image() {
    if(imageInfo)
        delete imageInfo;
}

void Image::lock() {
    mutex.lock();
}

void Image::unlock() {
    mutex.unlock();
}

QString Image::getPath() {
    return path;
}

bool Image::isLoaded() {
    return loaded;
}

ImageType Image::type() {
    return imageInfo->imageType();
}

ImageInfo *Image::info() {
    return imageInfo;
}
