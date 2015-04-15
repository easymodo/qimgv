#include "imageloader.h"

ImageLoader::ImageLoader(DirectoryManager *_dm) {
    cache = new ImageCache();
    dm = _dm;
    cache->init(dm->getFileList());
    readSettings();
    connect(globalSettings, SIGNAL(settingsChanged()),
            this, SLOT(readSettings()));
}

void ImageLoader::open(QString path) {
    if(!dm->existsInCurrentDir(path)) {
        dm->setFile(path);
        cache->init(dm->getFileList());
    } else {
        dm->setFile(path);
    }
    load(dm->currentFilePos());
}

void ImageLoader::load(int pos) {
    emit loadStarted();
    cache->loadAt(pos);
    emit loadFinished(cache->imageAt(pos));
    /*Image *img = new Image(file);
    setCurrentImg(img);
    QtConcurrent::run(this, &ImageLoader::load_thread, img);
    */
}

void ImageLoader::loadNext() {
    // dont do anything if already at last file
    if(dm->peekNext(1) != dm->currentFilePos()) {
        load(dm->nextPos());
        //free image at current-2 position
        int toUnload = dm->peekPrev(2);
        if(toUnload!=dm->currentFilePos() &&
           toUnload!=dm->peekPrev(1)) {
            cache->imageAt(toUnload)->unloadImage();
        }
    }
}

void ImageLoader::loadPrev() {
    // dont do anything if already at first file
    if(dm->peekPrev(1) != dm->currentFilePos()) {
        load(dm->prevPos());
        //free image at current+2 position
        int toUnload = dm->peekNext(2);
        if(toUnload!=dm->currentFilePos() &&
           toUnload!=dm->peekNext(1)) {
            cache->imageAt(toUnload)->unloadImage();
        }
    }
}

void ImageLoader::load_thread(Image* img) {
 /*   emit loadStarted();
    if(loadDelayEnabled) QThread::msleep(35);
    if(isCurrent(img)) {
        mutex2.lock();
        Image* found = cache->findImage(img);
        if(!found) {
            qDebug() << "loadStart: " << img->getFileName();
            img->loadImage();
            cache->cacheImageForced(img);
        }
        else {
            delete img;
            img = found;
        }
        img->setUseFlag(true);
        mutex2.unlock();
        emit loadFinished(img);
        emit startPreload();
    }
    */
}

void ImageLoader::preload(FileInfo *file) {
 /*   if (file->getFileSize()>=1.0) {
        Image* img = new Image(file);
        if(!cache->findImage(img)) { // not found; preloading
            QtConcurrent::run(this, &ImageLoader::preload_thread, img);
        }
    }
    */
}

void ImageLoader::preload_thread(Image* img) {
  /*  lock();
    qDebug() << "PreloadStart: " << img->getFileName();
    img->loadImage();
    if(!cache->cacheImageForced(img)) {
        delete img;
        img = NULL;
    }
    else {
        img->moveToThread(this->thread()); // important
    }
    unlock();
    */
}


void ImageLoader::readSettings() {
 /* loadDelayEnabled = globalSettings->s.value("loadDelay", "false").toBool();
    if(globalSettings->s.value("usePreloader", true).toBool()) {
        connect(this, SIGNAL(startPreload()),
                this, SLOT(preloadNearest()));
    }
    else {
        disconnect(this, SIGNAL(startPreload()),
                   this, SLOT(preloadNearest()));
    }
 */
}

void ImageLoader::lock() {
    mutex.lock();
}

void ImageLoader::unlock() {
    mutex.unlock();
}
