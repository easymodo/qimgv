#include "imagecache.h"

ImageCache::ImageCache() {
    cachedImages = new QList<CacheObject*>();
    applySettings();
    connect(globalSettings, SIGNAL(settingsChanged()),
            this, SLOT(applySettings()));
}

// call when changing directory
void ImageCache::init(QString directory, QStringList list) {
    dir = directory;
    // also should free memory
    lock();
    cachedImages->clear();
    for(int i=0; i<list.length(); i++) {
        cachedImages->append(new CacheObject(list.at(i)));
    }
    unlock();
    emit initialized(length());
}

QString ImageCache::currentDirectory() {
    return dir;
}

void ImageCache::insert(Image *img, int pos) {
    cachedImages->at(pos)->setImage(img);
}

// TO DELETE
void ImageCache::unloadAll() {
    lock();
    for(int i=0; i<cachedImages->length(); i++) {
        cachedImages->at(i)->unload();
    }
    unlock();
}

// TO DELETE
void ImageCache::unloadAt(int pos) {
    lock();
    //int time = clock();
    cachedImages->at(pos)->unload();
    //qDebug() << "cache unload: " << clock() - time;
    unlock();
}

// TO DELETE
void ImageCache::loadAt(int pos) {
    lock();
    cachedImages->at(pos)->load();
    unlock();
}

Image* ImageCache::imageAt(int pos) {
    return cachedImages->at(pos)->image();
}

bool ImageCache::isLoaded(int pos) {
    return cachedImages->at(pos)->isLoaded();
}

int ImageCache::currentlyLoadedCount() {
    int x =0;
    lock();
    for(int i=0; i<cachedImages->length(); i++) {
        if(isLoaded(i)) {
            x++;
        }
    }
    unlock();
    return x;
}

// TO DELETE
const Thumbnail* ImageCache::thumbnailAt(int pos) const {
    return cachedImages->at(pos)->getThumbnail();
}

// TO DELETE
void ImageCache::generateAllThumbnails() {
    QtConcurrent::map(*cachedImages, [](CacheObject* obj) {obj->generateThumbnail();} );
}

// TO DELETE
const FileInfo* ImageCache::infoAt(int pos) {
    return cachedImages->at(pos)->getInfo();
}

int ImageCache::length() const {
    return cachedImages->length();
}

void ImageCache::readSettings() {
    lock();
    //maxCacheSize = globalSettings->s.value("cacheSize").toInt();
    unlock();
}

void ImageCache::applySettings() {
    readSettings();
}

void ImageCache::lock() {
    mutex.lock();
}

void ImageCache::unlock() {
    mutex.unlock();
}

ImageCache::~ImageCache() {

}
