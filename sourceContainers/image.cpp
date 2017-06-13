#include "image.h"

void Image::safeDelete() {
    mutex.lock(); // ensure we are not used
    mutex.unlock();
    deleteLater();
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

fileType Image::type() {
    return fileInfo->getType();
}

FileInfo *Image::info() {
    return fileInfo;
}

void Image::attachInfo(FileInfo *_info) {
    fileInfo = _info;
}
