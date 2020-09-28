#include "thumbnailer.h"

Thumbnailer::Thumbnailer() {
    cache = new ThumbnailCache();
    pool = new QThreadPool(this);
    size = 100;
    crop = false;
    int threads = settings->thumbnailerThreadCount();
    int globalThreads = QThreadPool::globalInstance()->maxThreadCount();
    if(threads > globalThreads)
        threads = globalThreads;
    pool->setMaxThreadCount(threads);
}

Thumbnailer::~Thumbnailer() {
    bufferedTasks.clear();
    pool->waitForDone();
}

void Thumbnailer::setParameters(int _size, bool _crop) {
    bool reset = false;
    if(size != _size) {
        size = _size;
        reset = true;
    }
    if(crop != _crop) {
        crop = _crop;
        reset = true;
    }
    if(reset)
        clearAllTasks();
}

void Thumbnailer::clearQueuedTasks() {
    bufferedTasks.clear();

    /* For some reason QThreadPool::clear() [recently started?] crashing
     * when it has some number of tasks queued.
     * Crashes are occur more often when the queue is large.
     * To workaround we don't clear it at all, and instead just limit the amount
     * of tasks being put into queue by holding them in an another QList.
     * This causes some overhead & code looks crappier but it doesn't crash.
     */
    //pool->clear();
}

void Thumbnailer::clearAllTasks() {
    clearQueuedTasks();
    // this is called much less frequently & with alot less tasks; seems okay
    pool->clear();
}

std::shared_ptr<Thumbnail> Thumbnailer::getThumbnail(QString filePath, int size) {
    return ThumbnailerRunnable::generate(nullptr, filePath, size, false, false);
}

void Thumbnailer::getThumbnailAsync(QString path, bool force) {
    if(force) {
        bufferedTasks.removeAll(path);
        startThumbnailerThread(path, size, crop, force);
    } else {
        if(!runningTasks.contains(path) && !bufferedTasks.contains(path))
            bufferedTasks.append(path);
        tryToStartMoreThreads();
    }
}

void Thumbnailer::startThumbnailerThread(QString filePath, int size, bool crop, bool force) {
    auto runnable = new ThumbnailerRunnable( (settings->useThumbnailCache() && !force) ? cache : nullptr, filePath, size, crop, force );
    connect(runnable, &ThumbnailerRunnable::taskEnd, this, &Thumbnailer::onTaskEnd);
    runnable->setAutoDelete(true);
    runningTasks.push_back(filePath);
    pool->start(runnable);
}

void Thumbnailer::tryToStartMoreThreads() {
    while(!bufferedTasks.isEmpty()) {
        if(runningTasks.count() >= pool->maxThreadCount()) {
            // pool's closed.
            return;
        }
        startThumbnailerThread(bufferedTasks.takeFirst(), size, crop, false);
    }
}

void Thumbnailer::onTaskEnd(std::shared_ptr<Thumbnail> thumbnail, QString filePath) {
    runningTasks.removeOne(filePath);
    emit thumbnailReady(thumbnail, filePath);
    tryToStartMoreThreads();
}
