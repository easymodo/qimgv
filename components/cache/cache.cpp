#include "cache.h"

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

void ImageCache::init(QString _dir, QStringList *fileNameList) {
    lock();
    dir = _dir;
    while(!cachedImages->isEmpty()) {
        delete cachedImages->takeAt(0);
    }
    for(int i = 0; i < fileNameList->count(); i++) {
        cachedImages->append(new CacheObject(fileNameList->at(i)));
    }
    loadedIndexes.clear();
    unlock();
    emit initialized(length());
}

// todo: do a shift in loadedIndexes
void ImageCache::removeAt(int pos) {
    lock();
    CacheObject *img = cachedImages->takeAt(pos);
    delete img;
    loadedIndexes.removeAt(loadedIndexes.indexOf(pos));
    unlock();
    emit itemRemoved(pos);
}

void ImageCache::unloadAll() {
    lock();
    //qDebug() << "CACHE: Clear all ";
    for(int i = 0; i < cachedImages->length(); i++) {
        cachedImages->at(i)->unload();
    }
    loadedIndexes.clear();
    unlock();
}

void ImageCache::unloadAt(int pos) {
    lock();
    if(checkRange(pos)) {
        //qDebug() << "CACHE: Unload " << pos;
        cachedImages->at(pos)->unload();
        loadedIndexes.removeAt(loadedIndexes.indexOf(pos));
    }
    unlock();
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
    lock();
    if(checkRange(pos)) {
        CacheObject *img = cachedImages->at(pos);
        unlock();
        return img->isLoaded();
    }
    else {
        unlock();
        return false;
    }
}

QString ImageCache::currentlyLoadedCount() {
    lock();
    QString str = "[ ";
    int x = 0;
    for(int i = 0; i < loadedIndexes.length(); i++) {
        str.append(" " + QString::number(loadedIndexes.at(i)));
    }   
    str.append("  ]");
    unlock();
    return str;
}

const QList<int> ImageCache::currentlyLoadedList() {
    return loadedIndexes;
}

void ImageCache::setImage(Image *img, int pos) {
    lock();
    if(checkRange(pos)) {
        if(!cachedImages->at(pos)->isLoaded()) {
            cachedImages->at(pos)->setImage(img);
            loadedIndexes.append(pos);
        } else {
            qDebug() << "CACHE: Warning, trying to set already loaded image. Index was: " << pos <<". This is a bug in loader!";
        }
    }
    unlock();
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
