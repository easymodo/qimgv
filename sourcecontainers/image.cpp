#include "image.h"

Image::Image() : imageInfo(NULL) {
    sem = new QSemaphore(1);
}

Image::~Image() {
    if(imageInfo)
        delete imageInfo;
    delete sem;
}

void Image::lock() {
    sem->acquire();
}

void Image::unlock() {
    sem->release();
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
