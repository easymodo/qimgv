#include "newloader.h"

// ######## WARNING: spaghetti code ##########

NewLoader::NewLoader(DirectoryManager *_dm) :
    reduceRam(false),
    current(NULL),
    preloadTarget(0),
    loadTarget(-1)
{
    dm = _dm;
    loadThread = new QThread(this);
    readSettings();
   //QPixmapCache::setCacheLimit(20480);
    QThreadPool::globalInstance()->setMaxThreadCount(4);
    connect(settings, SIGNAL(settingsChanged()),
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
    //ugly shit
    int target = dm->currentFilePos();
    setLoadTarget(target);
    LoadHelper *localWorker = new LoadHelper(cache, thread());
    worker->setTarget(target, dm->currentFilePath());
    localWorker->setTarget(target, dm->currentFilePath());
    localWorker->doLoad();
    onLoadFinished(target);
    delete localWorker;
}

void NewLoader::open(int pos) {
    if(dm->setCurrentPos(pos)) {
        emit loadStarted();
        doLoad(dm->currentFilePos());
        freeAuto();
    }
}

void NewLoader::doLoad(int pos) {
    setLoadTarget(pos);
    if(!cache->isLoaded(pos)) {
        preloadTimer->stop();
        loadTimer->start(loadTimer->isActive() ? LOAD_DELAY : 0);
    } else {
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
        emit loadFinished(cache->imageAt(loaded), loaded);
        current = cache->imageAt(loaded);
    } else if(isRelevant(loaded)) {
        //qDebug() << "loadfinished image is relevant, keeping.." << loaded;
    } else {
        cache->unloadAt(loaded);
        //qDebug() << "load finished but not relevant ("<< loaded <<"). deleting..";
    }
}

bool NewLoader::isRelevant(int pos) {
    return !(pos < loadTarget - 1 || pos > loadTarget + 1);
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
    if(isRelevant(preloadTarget)) {
        worker->setTarget(preloadTarget, dm->filePathAt(preloadTarget));
        //qDebug()<< "PRELOAD " << worker->target();
        emit startLoad();
    }
    mutex.unlock();
}

void NewLoader::freeAuto() {
    for(int i = 0; i < cache->length(); i++) {
        if(!isRelevant(i) && cache->isLoaded(i)) {
            bool flag = false;
            if(cache->imageAt(i) == current) {
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
    cache->unloadAll();
    current = NULL;
}

void NewLoader::freeAt(int toUnload) {
    if(!isRelevant(toUnload)) {
        //qDebug() << "!!!deleting" << toUnload << "  "<< cache->imageAt(toUnload);
        if(current == cache->imageAt(toUnload)) {
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
    cache->init(dm->currentDirectory(), dm->fileList());
    worker = new LoadHelper(cache, this->thread());
    worker->moveToThread(loadThread);
    connect(this, SIGNAL(startLoad()), worker, SLOT(doLoad()));
    connect(worker, SIGNAL(finished(int)), this, SLOT(onLoadFinished(int)));

    loadTimer = new QTimer(this);
    loadTimer->setSingleShot(true);
    connect(loadTimer, SIGNAL(timeout()), this, SLOT(onLoadTimeout()));
    preloadTimer = new QTimer(this);
    preloadTimer->setSingleShot(true);
    if(settings->usePreloader()) {
        connect(preloadTimer, SIGNAL(timeout()), this, SLOT(onPreloadTimeout()));
    }
    loadThread->start();
}

void NewLoader::reinitCache() {
    if(cache->currentDirectory() != dm->currentDirectory()) {
        cache->init(dm->currentDirectory(), dm->fileList());
    }
}

void NewLoader::reinitCacheForced() {
    cache->init(dm->currentDirectory(), dm->fileList());
}

// for position in directory
void NewLoader::generateThumbnailFor(int pos) {
    Thumbnailer *thWorker = new Thumbnailer(cache, dm->filePathAt(pos), pos, settings->squareThumbnails());
    connect(thWorker, SIGNAL(thumbnailReady(int,Thumbnail*)),
            this, SIGNAL(thumbnailReady(int,Thumbnail*)));
    thWorker->setAutoDelete(true);
    QThreadPool::globalInstance()->start(thWorker);
    //QtConcurrent::run(this, &NewLoader::generateThumbnailThread, pos);
}

void NewLoader::readSettings() {
    if(settings->usePreloader()) {
        connect(this, SIGNAL(startPreload()),
                this, SLOT(doPreload()));
    } else {
        disconnect(this, SIGNAL(startPreload()),
                   this, SLOT(doPreload()));
    }
    reduceRam = settings->reduceRamUsage();
}

void NewLoader::lock() {
    mutex.lock();
}

void NewLoader::unlock() {
    mutex.unlock();
}




