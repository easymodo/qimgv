#include "newloader.h"

// ######## WARNING: spaghetti code ##########

NewLoader::NewLoader(DirectoryManager *_dm) :
    preloadTarget(0),
    currentIndex(-1)
{
    dm = _dm;
    readSettings();
    QThreadPool::globalInstance()->setMaxThreadCount(4);
    connect(settings, SIGNAL(settingsChanged()),
            this, SLOT(readSettings()));
}

// TODO
void NewLoader::openBlocking(int pos) {
    open(pos);
}

void NewLoader::open(int pos) {
    emit loadStarted();
    currentIndex = pos;
    freeAuto();
    if(tasks.contains(pos)) {
        return;
    }
    if(cache->isLoaded(pos)) {
        emit loadFinished(cache->imageAt(pos), pos);
        return;
    }
    LoaderRunnable *runnable = new LoaderRunnable(dm->filePathAt(pos), pos, thread());
    connect(runnable, SIGNAL(finished(Image*,int)), this, SLOT(onLoadFinished(Image*,int)), Qt::UniqueConnection);
    tasks.append(pos);
    QThreadPool::globalInstance()->start(runnable);
}

void NewLoader::preload(int pos) {
    preloadTarget = pos;
    preloadTimer->start();
}

void NewLoader::doPreload() {
    if(!cache->isLoaded(preloadTarget)) {
        if(tasks.contains(preloadTarget)) {
            return;
        }
        LoaderRunnable *runnable = new LoaderRunnable(dm->filePathAt(preloadTarget), preloadTarget, thread());
        connect(runnable, SIGNAL(finished(Image*,int)), this, SLOT(onLoadFinished(Image*,int)), Qt::UniqueConnection);
        tasks.append(preloadTarget);
        QThreadPool::globalInstance()->start(runnable);
    }
}

void NewLoader::onLoadFinished(Image *image, int index) {
    lock();
    tasks.removeAt(tasks.indexOf(index));
    if(isRelevant(index)) {
        if(!cache->isLoaded(index))
            cache->setImage(image, index);
        else
            delete image;
        if(index == currentIndex)
            emit loadFinished(cache->imageAt(index), index);
    } else {
        delete image;
    }
    unlock();
}

bool NewLoader::isRelevant(int pos) {
    return !(pos < currentIndex - 1 || pos > currentIndex + 1);
}

void NewLoader::freeAuto() {
    lock();
    const QList<int>* loadedList = cache->currentlyLoadedList();
    for(int i = 0; i < loadedList->length(); i++) {
        if(!isRelevant(loadedList->at(i))) {
            cache->unloadAt(loadedList->at(i));
        }
    }
    //qDebug()  << cache->currentlyLoadedList();
    unlock();
}

void NewLoader::freeAll() {
    cache->unloadAll();
}

bool NewLoader::setLoadTarget(int _target) {
    if(_target == currentIndex) {
        return false;
    }
    currentIndex = _target;
    return true;
}

const ImageCache *NewLoader::getCache() {
    return cache;
}

void NewLoader::setCache(ImageCache *_cache) {
    this->cache = _cache;
    preloadTimer = new QTimer(this);
    preloadTimer->setSingleShot(true);
    if(settings->usePreloader()) {
        connect(preloadTimer, SIGNAL(timeout()),
                this, SLOT(doPreload()), Qt::UniqueConnection);
    }
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
