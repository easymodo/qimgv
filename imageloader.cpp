#include "imageloader.h"

ImageLoader::ImageLoader(DirectoryManager *_dm) :
    reduceRam(false)
{
    dm = _dm;
    readSettings();
    // cache causes crash when creating thumbnails
    // will look into this later
    QPixmapCache::setCacheLimit(0);
    QThreadPool::globalInstance()->setMaxThreadCount(4),
    connect(globalSettings, SIGNAL(settingsChanged()),
            this, SLOT(readSettings()));
}

void ImageLoader::open(QString path) {
    cache->unloadAll();
    if(!dm->existsInCurrentDir(path)) {
        dm->setFile(path);
        reinitCache();
    } else {
        dm->setFile(path);
    }
    lock();
    QtConcurrent::run(this, &ImageLoader::doLoad, dm->currentFilePos());
}

void ImageLoader::reinitCache() {
    if(cache->currentDirectory() != dm->currentDirectory()) {
        cache->init(dm->currentDirectory(), dm->getFileList());
    }
}

void ImageLoader::reinitCacheForced() {
    cache->init(dm->currentDirectory(), dm->getFileList());
}

void ImageLoader::openBlocking(QString path) {
    cache->unloadAll();
    if(!dm->existsInCurrentDir(path)) {
        dm->setFile(path);
        reinitCache();
    } else {
        dm->setFile(path);
    }
    lock();
    doLoad(dm->currentFilePos());
}

void ImageLoader::open(int pos) {
    cache->unloadAll();
    dm->setCurrentPos(pos);
    lock();
    QtConcurrent::run(this, &ImageLoader::doLoad, dm->currentFilePos());
}

void ImageLoader::doLoad(int pos) {
    emit loadStarted();
    cache->loadAt(pos);
    emit loadFinished(cache->imageAt(pos), pos);
    unlock();
}

// for position in directory
void ImageLoader::generateThumbnailFor(int pos) {
    QtConcurrent::run(this, &ImageLoader::generateThumbnailThread, pos);
}

void ImageLoader::generateThumbnailThread(int pos) {
    const Thumbnail* pix = cache->thumbnailAt(pos);
    emit thumbnailReady(pos, pix);
}

void ImageLoader::loadNext() {
    // dont do anything if already at last file
    if(dm->peekNext(1) != dm->currentFilePos()) {
        lock();
        //free image at prev position
        freePrev();
        //switch to next image
        if(!cache->isLoaded(dm->nextPos())) {

            //QtConcurrent::run(this,
            //                  &ImageLoader::doLoad,
            //                  dm->currentFilePos());
            doLoad(dm->currentFilePos());
        }
        else {
            unlock();
            emit loadFinished(cache->imageAt(dm->currentFilePos()),
                              dm->currentFilePos());
        }
        //free prev image again
        if(reduceRam) {
            freePrev();
        }
        //start preloading next
        if(dm->peekNext(1)!=dm->currentFilePos()) {
            startPreload(dm->peekNext(1));
        }
    }
}

void ImageLoader::loadPrev() {
    // dont do anything if already at first file
    if(dm->peekPrev(1) != dm->currentFilePos()) {
        lock();
        //free image at next position
        freeNext();
        //switch to prev image
        if(!cache->isLoaded(dm->prevPos())) {
            //QtConcurrent::run(this,
            //                  &ImageLoader::doLoad,
            //                  dm->currentFilePos());
            doLoad(dm->currentFilePos());
        }
        else {
            unlock();
            emit loadFinished(cache->imageAt(dm->currentFilePos()),
                              dm->currentFilePos());
        }
        //free next image again
        if(reduceRam) {
            freeNext();
        }
        //start preloading prev
        if(dm->peekPrev(1)!=dm->currentFilePos()) {
            startPreload(dm->peekPrev(1));
        }
    }
}

void ImageLoader::freePrev() {
    int toUnload = dm->peekPrev(1);
    if(toUnload!=dm->currentFilePos()) {
        cache->unloadAt(toUnload);
    }
}

void ImageLoader::freeNext() {
    int toUnload = dm->peekNext(1);
    if(toUnload!=dm->currentFilePos()) {
        cache->unloadAt(toUnload);
    }
}

const ImageCache *ImageLoader::getCache() {
    return cache;
}

void ImageLoader::setCache(ImageCache *_cache) {
    this->cache = _cache;
    cache->init(dm->currentDirectory(), dm->getFileList());
}

void ImageLoader::preload(int pos) {
    QFuture<void> future = QtConcurrent::run(this, &ImageLoader::doPreload, pos);
}

void ImageLoader::doPreload(int pos) {
    cache->loadAt(pos);
}


void ImageLoader::readSettings() {
    if( globalSettings->usePreloader() ) {
        connect(this, SIGNAL(startPreload(int)),
                this, SLOT(preload(int)));
    }
    else {
        disconnect(this, SIGNAL(startPreload(int)),
                   this, SLOT(preload(int)));
    }
    reduceRam = globalSettings->reduceRamUsage();
}

void ImageLoader::lock() {
    mutex.lock();
}

void ImageLoader::unlock() {
    mutex.unlock();
}
