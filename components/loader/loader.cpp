#include "loader.h"

NewLoader::NewLoader(const DirectoryManager *_dm, Cache2 *_cache) :
    preloadTarget(0),
    currentIndex(-1),
    unloadMargin(1),
    cache(_cache),
    dm(_dm)
{
    dm = _dm;
    thumbnailCache = new ThumbnailCache();
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
    if(cache->contains(dm->fileNameAt(index))) {
        emit loadFinished(cache->get(dm->fileNameAt(index)), index);
        return;
    }
    LoaderRunnable *runnable = new LoaderRunnable(dm->filePathAt(index), index);
    connect(runnable, SIGNAL(finished(Image*,int)), this, SLOT(onLoadFinished(Image*,int)), Qt::UniqueConnection);
    runnable->setAutoDelete(true);
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
    if(cache->contains(dm->fileNameAt(index))) {
        emit loadFinished(cache->get(dm->fileNameAt(index)), index);
        return;
    }
    LoaderRunnable *runnable = new LoaderRunnable(dm->filePathAt(index), index);
    connect(runnable, SIGNAL(finished(Image*,int)), this, SLOT(onLoadFinished(Image*,int)), Qt::UniqueConnection);
    runnable->setAutoDelete(true);
    tasks.append(index);
    QThreadPool::globalInstance()->start(runnable);
}

void NewLoader::preload(int index) {
    preloadTarget = index;
    preloadTimer->start();
}

void NewLoader::doPreload() {
    cache->lock();
    if(!cache->contains(dm->fileNameAt(preloadTarget))) {
        if(!tasks.contains(preloadTarget)) {
            LoaderRunnable *runnable = new LoaderRunnable(dm->filePathAt(preloadTarget), preloadTarget);
            connect(runnable, SIGNAL(finished(Image*,int)), this, SLOT(onLoadFinished(Image*,int)), Qt::UniqueConnection);
            tasks.append(preloadTarget);
            QThreadPool::globalInstance()->start(runnable);
        }
    }
    cache->unlock();
}

// todo: simplify
void NewLoader::onLoadFinished(Image *image, int index) {
    cache->lock();
    tasks.removeAt(tasks.indexOf(index));
    if(isRelevant(index)) {
        QString nameKey = dm->fileNameAt(index);
        if(!cache->contains(nameKey))
            cache->insert(nameKey, image);
        else
            delete image;
        cache->unlock();
        if(index == currentIndex) {
            //cache->release(nameKey); // this one?
            emit loadFinished(cache->get(nameKey), index);
        }
    } else {
        cache->unlock();
        delete image;
    }

}

bool NewLoader::isRelevant(int index) {
    return !(index < currentIndex - unloadMargin || index > currentIndex + unloadMargin);
}

void NewLoader::freeAuto() {
    cache->lock();
    const QList<QString> loadedKeys = cache->keys();
    for(int i = 0; i < loadedKeys.length(); i++) {
        int index = dm->indexOf(loadedKeys.at(i));
        if(!isRelevant(index)) {
            cache->remove(loadedKeys.at(i));
        }
    }
    cache->unlock();
    /*
    cache->lock();
    const QList<int> loadedList = cache->currentlyLoadedList();
    for(int i = 0; i < loadedList.length(); i++) {
        if(!isRelevant(loadedList.at(i))) {
            cache->unloadAt(loadedList.at(i));
        }
    }
    cache->unlock();
    */
}

bool NewLoader::setLoadTarget(int _target) {
    if(_target == currentIndex) {
        return false;
    }
    currentIndex = _target;
    return true;
}

const Cache2 *NewLoader::getCache() {
    return cache;
}

void NewLoader::setCache(Cache2 *_cache) {
    this->cache = _cache;
}

// for position in directory
void NewLoader::generateThumbnailFor(int index, int size) {
    if(size > 0) {
        ThumbnailerRunnable *thumbnailerRunnable = new ThumbnailerRunnable(
                    thumbnailCache,
                    dm->filePathAt(index),
                    index,
                    size,
                    settings->squareThumbnails());
        connect(thumbnailerRunnable, SIGNAL(thumbnailReady(int, Thumbnail*)),
                this, SIGNAL(thumbnailReady(int, Thumbnail*)));
        thumbnailerRunnable->setAutoDelete(true);
        QThreadPool::globalInstance()->start(thumbnailerRunnable);
    } else {
        qDebug() << "Loader: incorrect thumbnail size, ignoring request.";
    }
}

void NewLoader::readSettings() {
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
}

void NewLoader::lock() {
    mutex.lock();
}

void NewLoader::unlock() {
    mutex.unlock();
}
