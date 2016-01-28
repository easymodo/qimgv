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
    while(!cachedImages->isEmpty()) {
        delete cachedImages->takeAt(0);
    }
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

Image *ImageCache::imageAt(unsigned int pos) {
    if(pos >= cachedImages->length() && !cachedImages->isEmpty())
        return NULL;
    return cachedImages->at(pos)->image();
}

int ImageCache::length() const {
    return cachedImages->length();
}


QString ImageCache::currentDirectory() {
    return dir;
}

bool ImageCache::isLoaded(int pos) {
    if(pos >=cachedImages->length())
        return false;
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
    if(pos >= cachedImages->length())
        return;
    cachedImages->at(pos)->setImage(img);
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
