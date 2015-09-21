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

void ImageLoader::onLoadFinished(Image* img, int pos) {
    if(pos == loadTarget)
        emit loadFinished(img, pos);
}

void ImageLoader::loadNext() {
    emit loadStarted();
    setLoadTarget(dm->peekNext(1));
    int localTarget = loadTarget;
    // dont do anything if already at last file
    if(dm->peekNext(1) != dm->currentFilePos()) {
        lock();
        freePrev();

        if(!cache->isLoaded(dm->nextPos()) && localTarget == loadTarget) {

            //QtConcurrent::run(this,
            //                  &ImageLoader::doLoad,
            //                  dm->currentFilePos());
            //doLoad(dm->currentFilePos());
///test

          /*  QThread *thread = new QThread();
            LoadHelper *worker = new LoadHelper(cache, dm->currentFilePos());
            worker->moveToThread(thread);

            //connect(worker, SIGNAL(error(QString)), this, SLOT(errorString(QString)));
            connect(thread, SIGNAL(started()), worker, SLOT(doLoad()));
            connect(worker, SIGNAL(finished(Image*,int)), this, SLOT(onLoadFinished(Image*,int)));
            connect(worker, SIGNAL(finished(Image*, int)), thread, SLOT(quit()));
            connect(worker, SIGNAL(finished(Image*, int)), worker, SLOT(deleteLater()));
            connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

            QTimer *timer = new QTimer();
            timer->setSingleShot(true);
            connect(timer, SIGNAL(timeout()), thread, SLOT(start()));
            connect(timer, SIGNAL(timeout()), timer, SLOT(deleteLater()));
            timer->start(100);
            */
            unlock();
///end
        }
        else {
            unlock();
            //emit loadFinished(cache->imageAt(loadTarget),
            //                  loadTarget);
        }
        //free prev image again
        if(reduceRam) {
            freePrev();
        }
        //start preloading next
       // if(dm->peekNext(1)!=dm->currentFilePos()) {
       //     startPreload(dm->peekNext(1));
       // }
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
       // if(dm->peekPrev(1)!=dm->currentFilePos()) {
       //     startPreload(dm->peekPrev(1));
      // }
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

void ImageLoader::setLoadTarget(int _target) {
    loadTarget = _target;
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
