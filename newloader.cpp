#include "newloader.h"

NewLoader::NewLoader(DirectoryManager *_dm) :
    reduceRam(false)
{
    qDebug() << "mainThread: " << this->thread();
    dm = _dm;
    loadThread = new QThread();
    readSettings();
    // cache causes crash when creating thumbnails
    // will look into this later
    QPixmapCache::setCacheLimit(0);
    QThreadPool::globalInstance()->setMaxThreadCount(4),
    connect(globalSettings, SIGNAL(settingsChanged()),
            this, SLOT(readSettings()));
}

void NewLoader::open(QString path) {
    cache->unloadAll();
    if(!dm->existsInCurrentDir(path)) {
        dm->setFile(path);
        reinitCache();
    } else {
        dm->setFile(path);
    }
    lock();
    QtConcurrent::run(this, &NewLoader::doLoad, dm->currentFilePos());
}

void NewLoader::reinitCache() {
    if(cache->currentDirectory() != dm->currentDirectory()) {
        cache->init(dm->currentDirectory(), dm->getFileList());
    }
}

void NewLoader::reinitCacheForced() {
    cache->init(dm->currentDirectory(), dm->getFileList());
}

void NewLoader::openBlocking(QString path) {
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

void NewLoader::open(int pos) {
    cache->unloadAll();
    dm->setCurrentPos(pos);
    lock();
    QtConcurrent::run(this, &NewLoader::doLoad, dm->currentFilePos());
}

void NewLoader::doLoad(int pos) {
    emit loadStarted();
    cache->loadAt(pos);
    emit loadFinished(cache->imageAt(pos), pos);
    unlock();
}

// for position in directory
void NewLoader::generateThumbnailFor(int pos) {
    QtConcurrent::run(this, &NewLoader::generateThumbnailThread, pos);
}

void NewLoader::generateThumbnailThread(int pos) {
    const Thumbnail* pix = cache->thumbnailAt(pos);
    emit thumbnailReady(pos, pix);
}

void NewLoader::onLoadFinished(int loaded) {
    if(loaded == loadTarget) {
        qDebug() << "load finished" << loaded << ". Displaying image!";
        emit loadFinished(cache->imageAt(loaded), loaded);
        //worker->deleteLater();
    } else {
        qDebug() << "load finished but unneeded" << loaded << " vs current: " << loadTarget;
    }
}

void NewLoader::onLoadTimeout() {
    if(worker->target() == loadTarget) {
        qDebug() << "load thread start " << loadTarget;
        loadThread->start();
    } else {
        qDebug() << "timer: target expired:" << worker->target();
    }
}

void NewLoader::loadNext() {
    emit loadStarted();
    setLoadTarget(dm->peekNext(1));
    int localTarget = loadTarget;
    qDebug() << "#####################";
    qDebug() << "load start " << loadTarget;
    int time = clock();

    if(current != loadTarget && dm->peekNext(1) != dm->currentFilePos()) {
        lock();

        freePrev();
        qDebug() << "free time: " << clock() - time;

        if(!cache->isLoaded(dm->nextPos())) {

            worker->setTarget(dm->currentFilePos(), dm->currentFilePath());
            if(loadTimer->isActive()) {
                loadTimer->start(35);
            } else {
                loadTimer->start(0);
            }
            unlock();
        }
        else {
            unlock();
            emit loadFinished(cache->imageAt(dm->currentFilePos()), dm->currentFilePos());
        }
        if(reduceRam) {
            freePrev();
        }
    }
}

void NewLoader::loadPrev() {
    emit loadStarted();
    setLoadTarget(dm->peekPrev(1));
    int localTarget = loadTarget;
    qDebug() << "#####################";
    qDebug() << "load start " << loadTarget;

    int time = clock();
    if(dm->peekPrev(1) != dm->currentFilePos()) {
        lock();

        freePrev();
        qDebug() << "free time: " << clock() - time;

        if(!cache->isLoaded(dm->prevPos())) {
            worker->setTarget(dm->currentFilePos(), dm->currentFilePath());
            if(loadTimer->isActive()) {
                loadTimer->start(35);
            } else {
                loadTimer->start(0);
            }
            unlock();
        }
        else {
            unlock();
            emit loadFinished(cache->imageAt(dm->currentFilePos()), dm->currentFilePos());
        }
        //free next image again
        if(reduceRam) {
            freeNext();
        }
    }
    qDebug() << "TOTAL time: " << clock() - time;
}

void NewLoader::freePrev() {
    int toUnload = dm->peekPrev(1);
    if(toUnload!=dm->currentFilePos()) {
        cache->unloadAt(toUnload);
    }
}

void NewLoader::freeNext() {
    int toUnload = dm->peekNext(1);
    if(toUnload!=dm->currentFilePos()) {
        cache->unloadAt(toUnload);
    }
}

void NewLoader::setLoadTarget(int _target) {
    loadTarget = _target;
}

const ImageCache *NewLoader::getCache() {
    return cache;
}

void NewLoader::setCache(ImageCache *_cache) {
    this->cache = _cache;
    cache->init(dm->currentDirectory(), dm->getFileList());
    worker = new LoadHelper(cache, this->thread());
    worker->moveToThread(loadThread);
    ////connect(worker, SIGNAL(error(QString)), this, SLOT(errorString(QString)));
    connect(loadThread, SIGNAL(started()), worker, SLOT(doLoad()));
    connect(worker, SIGNAL(finished(int)), this, SLOT(onLoadFinished(int)));
    connect(worker, SIGNAL(finished(int)), loadThread, SLOT(quit()));
    //connect(worker, SIGNAL(finished(Image*, int)), worker, SLOT(deleteLater()));
    //connect(loadThread, SIGNAL(finished()), loadThread, SLOT(deleteLater()));

    loadTimer = new QTimer();
    loadTimer->setSingleShot(true);
    connect(loadTimer, SIGNAL(timeout()), this, SLOT(onLoadTimeout()));
    //connect(loadTimer, SIGNAL(timeout()), loadTimer, SLOT(deleteLater()));

}

void NewLoader::preload(int pos) {
    QFuture<void> future = QtConcurrent::run(this, &NewLoader::doPreload, pos);
}

void NewLoader::doPreload(int pos) {
    cache->loadAt(pos);
}


void NewLoader::readSettings() {
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

void NewLoader::lock() {
    mutex.lock();
}

void NewLoader::unlock() {
    mutex.unlock();
}
