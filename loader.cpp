#include "loader.h"

NewLoader::NewLoader(const DirectoryManager *_dm) :
    preloadTarget(0),
    currentIndex(-1),
    unloadMargin(1)
{
    dm = _dm;
    preloadTimer = new QTimer(this);
    preloadTimer->setSingleShot(true);
    readSettings();
    connect(settings, SIGNAL(settingsChanged()),
            this, SLOT(readSettings()));
}

void NewLoader::openBlocking(int index) {
    emit loadStarted();
    currentIndex = index;
    freeAuto();
    if(tasks.contains(index)) {
        return;
    }
    if(cache->isLoaded(index)) {
        emit loadFinished(cache->imageAt(index), index);
        return;
    }
    LoaderRunnable *runnable = new LoaderRunnable(dm->filePathAt(index), index, thread());
    connect(runnable, SIGNAL(finished(Image*,int)), this, SLOT(onLoadFinished(Image*,int)), Qt::UniqueConnection);
    tasks.append(index);
    runnable->run();
}

void NewLoader::open(int index) {
    emit loadStarted();
    currentIndex = index;
    freeAuto();
    if(tasks.contains(index)) {
        return;
    }
    if(cache->isLoaded(index)) {
        emit loadFinished(cache->imageAt(index), index);
        return;
    }
    LoaderRunnable *runnable = new LoaderRunnable(dm->filePathAt(index), index, thread());
    connect(runnable, SIGNAL(finished(Image*,int)), this, SLOT(onLoadFinished(Image*,int)), Qt::UniqueConnection);
    tasks.append(index);
    QThreadPool::globalInstance()->start(runnable);
}

void NewLoader::preload(int index) {
    preloadTarget = index;
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

// TODO: what if cache was reinitialized during loading?
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
    //qDebug()<< cache->currentlyLoadedList();
    unlock();
}

bool NewLoader::isRelevant(int index) {
    return !(index < currentIndex - unloadMargin || index > currentIndex + unloadMargin);
}

void NewLoader::freeAuto() {
    lock();
    const QList<int> loadedList = cache->currentlyLoadedList();
    for(int i = 0; i < loadedList.length(); i++) {
        if(!isRelevant(loadedList.at(i))) {
            cache->unloadAt(loadedList.at(i));
        }
    }
    unlock();
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
}

// for position in directory
void NewLoader::generateThumbnailFor(int index, long thumbnailId) {
    Thumbnailer *thumbnailerRunnable = new Thumbnailer(cache,
                                            dm->filePathAt(index),
                                            index,
                                            settings->squareThumbnails(),
                                            thumbnailId);
    connect(thumbnailerRunnable, SIGNAL(thumbnailReady(long, Thumbnail*)),
            this, SIGNAL(thumbnailReady(long, Thumbnail*)));
    thumbnailerRunnable->setAutoDelete(true);
    QThreadPool::globalInstance()->start(thumbnailerRunnable);
}

void NewLoader::readSettings() {
    lock();
    if(settings->usePreloader()) {
        unloadMargin = 1;
        connect(preloadTimer, SIGNAL(timeout()),
                this, SLOT(doPreload()), Qt::UniqueConnection);
    }
    else {
        unloadMargin = 0;
        disconnect(preloadTimer, SIGNAL(timeout()),
                   this, SLOT(doPreload()));
    }
    unlock();
}

void NewLoader::lock() {
    mutex.lock();
}

void NewLoader::unlock() {
    mutex.unlock();
}
