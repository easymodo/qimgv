#include "imagecache.h"

ImageCache::ImageCache() {
    cachedImages = new QList<CacheObject *>();
    applySettings();
    connect(settings, SIGNAL(settingsChanged()),
            this, SLOT(applySettings()));
}

ImageCache::~ImageCache() {
    delete cachedImages;
}

// ##############################################################
// ####################### PUBLIC METHODS #######################
// ##############################################################

void ImageCache::init(QString directory, QStringList list) {
    lock();
    dir = directory;
    cachedImages->clear();
    for(int i = 0; i < list.length(); i++) {
        cachedImages->append(new CacheObject(list.at(i)));
    }
    unlock();
    emit initialized(length());
}

void ImageCache::unloadAll() {
    lock();
    for(int i = 0; i < cachedImages->length(); i++) {
        cachedImages->at(i)->unload();
    }
    unlock();
}

void ImageCache::unloadAt(int pos) {
    lock();
    cachedImages->at(pos)->unload();
    unlock();
}

Image *ImageCache::imageAt(int pos) {
    return cachedImages->at(pos)->image();
}

Thumbnail *ImageCache::thumbnailAt(int pos) const {
    return cachedImages->at(pos)->getThumbnail();
}

int ImageCache::length() const {
    return cachedImages->length();
}


QString ImageCache::currentDirectory() {
    return dir;
}

bool ImageCache::isLoaded(int pos) {
    return cachedImages->at(pos)->isLoaded();
}

int ImageCache::currentlyLoadedCount() {
    int x = 0;
    lock();
    for(int i = 0; i < cachedImages->length(); i++) {
        if(isLoaded(i)) {
            x++;
        }
    }
    unlock();
    return x;
}

void ImageCache::setImage(Image *img, int pos) {
    cachedImages->at(pos)->setImage(img);
}

void ImageCache::setThumbnail(Thumbnail *thumb, int pos) {
    cachedImages->at(pos)->setThumbnail(thumb);
}




// ##############################################################
// ###################### PRIVATE METHODS #######################
// ##############################################################

void ImageCache::lock() {
    mutex.lock();
}

void ImageCache::unlock() {
    mutex.unlock();
}

void ImageCache::readSettings() {
    lock();
    //maxCacheSize = globalSettings->s.value("cacheSize").toInt();
    unlock();
}

// ##############################################################
// ###################### PRIVATE SLOTS #########################
// ##############################################################

void ImageCache::applySettings() {
    readSettings();
}
