#include "newloader.h"

NewLoader::NewLoader(DirectoryManager *_dm) :
    reduceRam(false),
    current(NULL),
    preloadTarget(0)
{
    dm = _dm;
    loadThread = new QThread(this);
    readSettings();
    //QPixmapCache::setCacheLimit(0);
    QThreadPool::globalInstance()->setMaxThreadCount(4),
    connect(globalSettings, SIGNAL(settingsChanged()),
            this, SLOT(readSettings()));
}

void NewLoader::open(QString path) {
    if(!dm->existsInCurrentDir(path)) {
        freeAll();
        preloadTarget = 0;
        loadTarget = 0;
        dm->setFile(path);
        reinitCache();
    } else {
        freeAuto();
        dm->setFile(path);
    }
    emit loadStarted();
    doLoad(dm->currentFilePos());
}

void NewLoader::openBlocking(QString path) {
    freeAuto();
    if(!dm->existsInCurrentDir(path)) {
        dm->setFile(path);
        reinitCache();
    } else {
        dm->setFile(path);
    }
    emit loadStarted();
    doLoad(dm->currentFilePos());
}

void NewLoader::open(int pos) {
    //cache->unloadAll();
    dm->setCurrentPos(pos);
    emit loadStarted();
    doLoad(dm->currentFilePos());
    freeAuto();
}

void NewLoader::doLoad(int pos) {
    time = clock();
    setLoadTarget(pos);
    if(!cache->isLoaded(pos)) {
        //worker->setTarget(pos, dm->filePathAt(pos));
       //qDebug() << "DO_LOAD: start timer for " << pos;
        preloadTimer->stop();
        loadTimer->start(loadTimer->isActive() ? 20 : 0);
    }
    else {
       //qDebug() << "found" << pos << ", skipping load.";
        emit onLoadFinished(pos);
    }
}

void NewLoader::doPreload() {
    if(!cache->isLoaded(preloadTarget)) {
        preloadTimer->start(200);
    }
}

void NewLoader::loadNext() {
    QMutexLocker locker(&mutex);
    if(dm->peekNext(1) != dm->currentFilePos()) {
        if(setLoadTarget(dm->nextPos())) {
            emit loadStarted();
            preloadTarget = dm->peekNext(1);
            doLoad(loadTarget);
            freeAuto();
        }
    }
}

void NewLoader::loadPrev() {
    QMutexLocker locker(&mutex);
    if(dm->peekPrev(1) != dm->currentFilePos()) {
        if(setLoadTarget(dm->prevPos())) {
            emit loadStarted();
            preloadTarget = dm->peekPrev(1);
            doLoad(loadTarget);
            freeAuto();
        }
    }
}

void NewLoader::onLoadFinished(int loaded) {
    startPreload();
    if(loaded == loadTarget && current != cache->imageAt(loaded)) {
        current = cache->imageAt(loaded);
       //qDebug() << "loadFinished: setting new current to " << loaded;
        emit loadFinished(cache->imageAt(loaded), loaded);
    } else if(isRevelant(loaded)) {
       //qDebug() << "loadfinished image is revelant, keeping.." << loaded;
    } else {
        cache->unloadAt(loaded);
       //qDebug() << "load finished but not revelant ("<< loaded <<"). deleting..";
    }
}

bool NewLoader::isRevelant(int pos) {
    return !(pos<loadTarget-1 || pos>loadTarget+1);
}

void NewLoader::onLoadTimeout() {
    mutex.lock();
    if(loadTarget != worker->target()) {
        worker->setTarget(loadTarget, dm->filePathAt(loadTarget));
        emit startLoad();
    }

    mutex.unlock();
}

void NewLoader::onPreloadTimeout() {
    mutex.lock();
   if(isRevelant(preloadTarget)) {
        worker->setTarget(preloadTarget, dm->filePathAt(preloadTarget));
       //qDebug()<< "PRELOAD " << worker->target();
        emit startLoad();
    }
    mutex.unlock();
}

void NewLoader::freeAuto() {
    for(int i=0; i<cache->length(); i++) {
        if(!isRevelant(i) && cache->isLoaded(i)) {
            bool flag = false;
            if(cache->imageAt(i) == current) {
                emit currentImageUnloading();
                flag = true;

            }
           //qDebug() <<"!!unloading: " << i;
            cache->unloadAt(i);
            if(flag)
                current = NULL;
        }
    }
}

void NewLoader::freeAll() {
    if(current) {
        emit currentImageUnloading();
    }
    cache->unloadAll();
    current = NULL;
}

void NewLoader::freeAt(int toUnload) {
    if(!isRevelant(toUnload)) {
       //qDebug() << "!!!deleting" << toUnload << "  "<< cache->imageAt(toUnload);
        if(current == cache->imageAt(toUnload)) {
            emit currentImageUnloading();
            current = NULL;
        }
        cache->unloadAt(toUnload);
       //qDebug() << "image freed: "<< toUnload;
    }
}

bool NewLoader::setLoadTarget(int _target) {
    if(_target == loadTarget) {
        return false;
    }
    loadTarget = _target;
    return true;
}

const ImageCache *NewLoader::getCache() {
    return cache;
}

void NewLoader::setCache(ImageCache *_cache) {
    this->cache = _cache;
    cache->init(dm->currentDirectory(), dm->getFileList());
    worker = new LoadHelper(cache, this->thread());
    worker->moveToThread(loadThread);
    //connect(worker, SIGNAL(error(QString)), this, SLOT(errorString(QString)));
    //connect(loadThread, SIGNAL(started()), worker, SLOT(doLoad()));
    connect(this, SIGNAL(startLoad()), worker, SLOT(doLoad()));
    connect(worker, SIGNAL(finished(int)), this, SLOT(onLoadFinished(int)));
    //connect(worker, SIGNAL(finished(int)), loadThread, SLOT(quit()));
    //connect(worker, SIGNAL(finished(Image*, int)), worker, SLOT(deleteLater()));
    //connect(loadThread, SIGNAL(finished()), loadThread, SLOT(deleteLater()));

    loadTimer = new QTimer(this);
    loadTimer->setSingleShot(true);
    connect(loadTimer, SIGNAL(timeout()), this, SLOT(onLoadTimeout()));
    preloadTimer = new QTimer(this);
    preloadTimer->setSingleShot(true);
    if(globalSettings->usePreloader()) {
            connect(preloadTimer, SIGNAL(timeout()), this, SLOT(onPreloadTimeout()));
    }
    loadThread->start();
    //connect(loadTimer, SIGNAL(timeout()), loadTimer, SLOT(deleteLater()));

}

void NewLoader::reinitCache() {
    if(cache->currentDirectory() != dm->currentDirectory()) {
        cache->init(dm->currentDirectory(), dm->getFileList());
    }
}

void NewLoader::reinitCacheForced() {
    cache->init(dm->currentDirectory(), dm->getFileList());
}

// for position in directory
void NewLoader::generateThumbnailFor(int pos) {
    Thumbnailer *thWorker = new Thumbnailer(cache, dm->filePathAt(pos), pos);
    connect(thWorker, SIGNAL(thumbnailReady(int)), this, SLOT(onThumbnailReady(int)));
    thWorker->setAutoDelete(true);
    QThreadPool::globalInstance()->start(thWorker);
    //QtConcurrent::run(this, &NewLoader::generateThumbnailThread, pos);
}

void NewLoader::onThumbnailReady(int pos) {
    emit thumbnailReady(pos, cache->thumbnailAt(pos));
}

void NewLoader::readSettings() {
    if( globalSettings->usePreloader() ) {
        connect(this, SIGNAL(startPreload()),
                this, SLOT(doPreload()));
    }
    else {
        disconnect(this, SIGNAL(startPreload()),
                   this, SLOT(doPreload()));
    }
    reduceRam = globalSettings->reduceRamUsage();
}

void NewLoader::lock() {
    mutex.lock();
}

void NewLoader::unlock() {
    mutex.unlock();
}
