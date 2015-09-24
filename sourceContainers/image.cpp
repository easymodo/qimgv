#include "image.h"

fileType Image::guessType() {
    QString ext = getExtension(path);
    if(ext == "webm" || ext == "gifv") { // case sensitivity?
        type = VIDEO;
        return type;
    }

    QFile file(path);
    file.open(QIODevice::ReadOnly);
    //read first 2 bytes to determine file format
    QByteArray startingBytes= file.read(2).toHex();
    file.close();

    if(startingBytes=="4749") {
        type=GIF;
        extension="GIF";
    }
    else if(startingBytes=="ffd8") {
        type=STATIC;
        extension="JPG";
    }
    else if(startingBytes=="8950") {
        type=STATIC;
        extension="PNG";
    }
    else if(startingBytes=="424d") {
        type=STATIC;
        extension="BMP";
    }
    else type = STATIC;
    return type;
}

void Image::safeDeleteSelf() {
    mutex.lock(); // ensure we are not used
    delete this;
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

fileType Image::getType() {
    return type;
}

FileInfo *Image::getInfo() {
    return info;
}

void Image::attachInfo(FileInfo *_info) {
    info = _info;
}
