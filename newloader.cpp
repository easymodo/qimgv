#include "newloader.h"

NewLoader::NewLoader(DirectoryManager *_dm) :
    reduceRam(false),
    current(NULL)
{
    dm = _dm;
    loadThread = new QThread(this);
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
    emit loadStarted();
    freeAll();
    doLoad(dm->currentFilePos());
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
    emit loadStarted();
    freeAll();
    doLoad(dm->currentFilePos());
}

void NewLoader::open(int pos) {
    //cache->unloadAll();
    dm->setCurrentPos(pos);
    emit loadStarted();
    freeAll();
    doLoad(dm->currentFilePos());
    //QtConcurrent::run(this, &NewLoader::doLoad, dm->currentFilePos());
}

void NewLoader::doLoad(int pos) {
    time = clock();
    setLoadTarget(pos);
    if(!cache->isLoaded(pos)) {
        //worker->setTarget(pos, dm->filePathAt(pos));
        qDebug() << "DO_LOAD: start timer for " << pos;
        preloadTimer->stop();
        if(loadTimer->isActive()) {
            loadTimer->start(150);
        } else {
            loadTimer->start(0);
        }
    }
    else {
        emit onLoadFinished(pos);
        qDebug() << "found" << pos << ", skipping load.";
    }
}

void NewLoader::preload(int pos) {

}

void NewLoader::doPreload(int pos) {
    if(!cache->isLoaded(pos)) {
        preloadTarget = pos;
        preloadTimer->start(100);
    }
}

void NewLoader::loadNext() {
    //QMutexLocker locker(&mutex);
    if(dm->peekNext(1) != dm->currentFilePos()) {
        //freePrev();
        if(setLoadTarget(dm->nextPos())) {
            qDebug() << "nextEvent";
            //setLoadTarget(dm->nextPos());
            freeAt(dm->peekPrev(2));
            //freeAt(dm->peekPrev(2));
            emit loadStarted();
            doLoad(loadTarget);
            doPreload(dm->peekNext(1));
            if(reduceRam) {
            //    freeAt(dm->peekPrev(1));
            }
        }
    }
}

void NewLoader::loadPrev() {
    //QMutexLocker locker(&mutex);
    if(dm->peekPrev(1) != dm->currentFilePos()) {
        //freeNext();
        setLoadTarget(dm->prevPos());
        //freeAt(dm->peekNext(1));
        freeAt(dm->peekNext(2));
        emit loadStarted();
        doLoad(loadTarget);
        doPreload(dm->peekPrev(1));
        //free next image again
        if(reduceRam) {
            //freeNext();
        }
    }
}

void NewLoader::onLoadFinished(int loaded) {
    if(loaded == loadTarget) {
        current = cache->imageAt(loaded);
        qDebug() << loaded << ": " << clock() - time;
        emit loadFinished(cache->imageAt(loaded), loaded);
    } else if(isRevelant(loaded)) {
        qDebug() << "loaded image isnt current but revelant, keeping.." << loaded;
    } else {
        cache->unloadAt(loaded);
        qDebug() << "load finished but unneeded ("<< loaded <<"). deleting..";
    }
}

bool NewLoader::isRevelant(int pos) {
    if(pos<loadTarget-1 || pos>loadTarget+1) {
        return false;
    } else {
        return true;
    }
}

void NewLoader::onLoadTimeout() {
    mutex.lock();
    if(loadTarget != worker->target()) {
        qDebug() << "###### " << loadTarget << worker->target() << "(setting worker target)";
        worker->setTarget(loadTarget, dm->filePathAt(loadTarget));
        emit startLoad();
    }

    mutex.unlock();
}

void NewLoader::onPreloadTimeout() {
    mutex.lock();
   if(isRevelant(preloadTarget)) {
        worker->setTarget(preloadTarget, dm->filePathAt(preloadTarget));
        qDebug()<< "starting preload for" << worker->target();
        emit startLoad();
    }
    mutex.unlock();
}

void NewLoader::freeAll() {
    cache->unloadAll();
    current = NULL;
}

void NewLoader::freeAt(int toUnload) {
    if(!isRevelant(toUnload)) {
        qDebug() << "!!!deleting" << toUnload << "  "<< cache->imageAt(toUnload);
        if(current == cache->imageAt(toUnload)) {
            emit currentImageUnloading();
            cache->unloadAt(toUnload);
            current = NULL;
        } else {
            cache->unloadAt(toUnload);
        }
        qDebug() << "image freed: "<< toUnload;
    }
}

bool NewLoader::setLoadTarget(int _target) {
    if(_target == loadTarget) {
        return false;
    } else {
        loadTarget = _target;
        return true;
    }
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
