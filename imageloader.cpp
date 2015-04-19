#include "imageloader.h"

ImageLoader::ImageLoader(DirectoryManager *_dm) {
    dm = _dm;
    readSettings();
    connect(globalSettings, SIGNAL(settingsChanged()),
            this, SLOT(readSettings()));
}

void ImageLoader::open(QString path) {
    cache->unloadAll();
    if(!dm->existsInCurrentDir(path)) {
        dm->setFile(path);
        cache->init(dm->getFileList());
    } else {
        dm->setFile(path);
    }
    lock();
    QtConcurrent::run(this, &ImageLoader::load_thread, dm->currentFilePos());
}

void ImageLoader::open(int pos) {
    cache->unloadAll();
    dm->setCurrentPos(pos);
    lock();
    QtConcurrent::run(this, &ImageLoader::load_thread, dm->currentFilePos());
}

void ImageLoader::load_thread(int pos) {
    emit loadStarted();
    cache->loadAt(pos);
    emit loadFinished(cache->imageAt(pos));
    unlock();
}



void ImageLoader::loadNext() {
    // dont do anything if already at last file
    if(dm->peekNext(1) != dm->currentFilePos()) {
        lock();
        //free image at prev position
        int toUnload = dm->peekPrev(1);
        if(toUnload!=dm->currentFilePos()) {
            cache->imageAt(toUnload)->unloadImage();
        }
        QtConcurrent::run(this, &ImageLoader::load_thread, dm->nextPos());
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
        int toUnload = dm->peekNext(1);
        if(toUnload!=dm->currentFilePos()) {
            cache->imageAt(toUnload)->unloadImage();
        }
        QtConcurrent::run(this, &ImageLoader::load_thread, dm->prevPos());
        if(dm->peekPrev(1)!=dm->currentFilePos()) {
            startPreload(dm->peekPrev(1));
        }
    }
}

const ImageCache *ImageLoader::getCache() {
    return cache;
}

void ImageLoader::setCache(ImageCache *_cache) {
    this->cache = _cache;
    cache->init(dm->getFileList());
}

void ImageLoader::preload(int pos) {
    QFuture<void> future = QtConcurrent::run(this, &ImageLoader::preload_thread, pos);
}

void ImageLoader::preload_thread(int pos) {
    //qDebug() << "preloadStart: " << cache->imageAt(pos)->getPath();
    cache->loadAt(pos);
}


void ImageLoader::readSettings() {
    if(globalSettings->s.value("usePreloader", true).toBool()) {
        connect(this, SIGNAL(startPreload(int)),
                this, SLOT(preload(int)));
    }
    else {
        disconnect(this, SIGNAL(startPreload(int)),
                   this, SLOT(preload(int)));
    }
}

void ImageLoader::lock() {
    mutex.lock();
}

void ImageLoader::unlock() {
    mutex.unlock();
}
