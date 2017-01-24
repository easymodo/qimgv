#include "imagecache.h"

ImageCache::ImageCache(DirectoryManager*_dm) : dm(_dm) {
    cachedImages = new QList<CacheObject *>();
    applySettings();
    connect(settings, SIGNAL(settingsChanged()),
            this, SLOT(applySettings()));

    connect(dm, SIGNAL(directorySortingChanged()), this, SLOT(init()));
}

ImageCache::~ImageCache() {
    delete cachedImages;
}

// ##############################################################
// ####################### PUBLIC METHODS #######################
// ##############################################################

void ImageCache::init() {
    if(!dm->currentDirectory().isEmpty()) {
        lock();
        dir = dm->currentDirectory();
        while(!cachedImages->isEmpty()) {
            delete cachedImages->takeAt(0);
        }
        for(int i = 0; i < dm->fileCount(); i++) {
            cachedImages->append(new CacheObject(dm->filePathAt(i)));
        }
        unlock();
        emit initialized(length());
    }
}

void ImageCache::removeAt(int pos) {
    lock();
    CacheObject *img = cachedImages->takeAt(pos);
    delete img;
    unlock();
    emit itemRemoved(pos);
}

void ImageCache::unloadAll() {
    lock();
    for(int i = 0; i < cachedImages->length(); i++) {
        cachedImages->at(i)->unload();
    }
    unlock();
}

void ImageCache::unloadAt(int pos) {
    if(checkRange(pos)) {
        lock();
        cachedImages->at(pos)->unload();
        unlock();
    }
}

Image *ImageCache::imageAt(int pos) {
    if(checkRange(pos))
        return cachedImages->at(pos)->image();
    else
        return NULL;
}

int ImageCache::length() const {
    return cachedImages->length();
}


QString ImageCache::currentDirectory() {
    return dir;
}

bool ImageCache::isLoaded(int pos) {
    CacheObject *img;
    lock();
    if(checkRange(pos)) {
        img = cachedImages->at(pos);
        // something bad happened and we have wrong file at pos
        // unload & set correct path
        if(img->filePath() != dm->filePathAt(pos)) {
            img->unload();
            img->setPath(dm->filePathAt(pos));
        }
        unlock();
        return img->isLoaded();
    }
    else {
        unlock();
        return false;
    }
}

int ImageCache::currentlyLoadedCount() {
    int x = 0;
    for(int i = 0; i < cachedImages->length(); i++) {
        if(isLoaded(i)) {
            x++;
        }
    }   
    return x;
}

void ImageCache::setImage(Image *img, int pos) {
    if(checkRange(pos)) {
        cachedImages->at(pos)->setImage(img);
    }
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

bool ImageCache::checkRange(int pos) {
    if(pos >= 0 && pos < cachedImages->length())
        return true;
    else
        return false;
}

// ##############################################################
// ###################### PRIVATE SLOTS #########################
// ##############################################################

void ImageCache::applySettings() {
    readSettings();
}
