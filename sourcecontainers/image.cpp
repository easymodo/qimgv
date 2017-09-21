#include "image.h"

Image::Image() : imageInfo(NULL) {
}

Image::~Image() {
    if(imageInfo)
        delete imageInfo;
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

QString Image::name() {
    return imageInfo->fileName();
}

ImageInfo *Image::info() {
    return imageInfo;
}
