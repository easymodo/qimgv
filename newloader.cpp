#include "newloader.h"

// ######## WARNING: spaghetti code ##########

NewLoader::NewLoader(DirectoryManager *_dm) :
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

void NewLoader::openBlocking(int pos) {
    freeAuto();
    emit loadStarted();
    //ugly shit
    int target = pos;
    setLoadTarget(target);
    LoadHelper *localWorker = new LoadHelper(cache, thread());
    worker->setTarget(target, dm->filePathAt(pos));
    localWorker->setTarget(target, dm->filePathAt(pos));
    localWorker->doLoad();
    onLoadFinished(target);
    delete localWorker;
}

void NewLoader::open(int pos) {
    emit loadStarted();
    worker->setTarget(-1, ""); // dafuq is this?
    doLoad(pos);
    freeAuto();
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

void NewLoader::preload(int pos) {
    preloadTarget = pos;
    emit startPreload();
}

void NewLoader::doPreload() {
    if(!cache->isLoaded(preloadTarget)) {
        preloadTimer->start(200);
    }
}

void NewLoader::onLoadFinished(int loaded) {
    if(loaded == loadTarget) {
        emit loadFinished(cache->imageAt(loaded), loaded);
        current = cache->imageAt(loaded);
        emit startPreload();
    } else {
        cache->unloadAt(loaded);
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
        if(current == cache->imageAt(toUnload)) {
            current = NULL;
        }
        cache->unloadAt(toUnload);
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
    worker = new LoadHelper(cache, this->thread());
    worker->moveToThread(loadThread);
    connect(this, SIGNAL(startLoad()),
            worker, SLOT(doLoad())), Qt::UniqueConnection;
    connect(worker, SIGNAL(finished(int)),
            this, SLOT(onLoadFinished(int)), Qt::UniqueConnection);

    loadTimer = new QTimer(this);
    loadTimer->setSingleShot(true);
    connect(loadTimer, SIGNAL(timeout()),
            this, SLOT(onLoadTimeout()), Qt::UniqueConnection);
    preloadTimer = new QTimer(this);
    preloadTimer->setSingleShot(true);
    if(settings->usePreloader()) {
        connect(preloadTimer, SIGNAL(timeout()),
                this, SLOT(onPreloadTimeout()), Qt::UniqueConnection);
    }
    loadThread->start();
}

// for position in directory
void NewLoader::generateThumbnailFor(int pos, long thumbnailId) {
    Thumbnailer *thWorker = new Thumbnailer(cache,
                                            dm->filePathAt(pos),
                                            pos,
                                            settings->squareThumbnails(),
                                            thumbnailId);
    connect(thWorker, SIGNAL(thumbnailReady(long, Thumbnail*)),
            this, SIGNAL(thumbnailReady(long, Thumbnail*)));
    thWorker->setAutoDelete(true);
    QThreadPool::globalInstance()->start(thWorker);
    //QtConcurrent::run(this, &NewLoader::generateThumbnailThread, pos);
}

void NewLoader::readSettings() {
    if(settings->usePreloader()) {
        connect(this, SIGNAL(startPreload()),
                this, SLOT(doPreload()), Qt::UniqueConnection);
    } else {
        disconnect(this, SIGNAL(startPreload()),
                   this, SLOT(doPreload()));
    }
}

void NewLoader::lock() {
    mutex.lock();
}

void NewLoader::unlock() {
    mutex.unlock();
}




