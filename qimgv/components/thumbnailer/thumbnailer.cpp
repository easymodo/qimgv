#include "thumbnailer.h"

Thumbnailer::Thumbnailer(DirectoryManager *_dm) : dm(_dm) {
    cache = new ThumbnailCache();
    pool = new QThreadPool(this);
    int threads = settings->thumbnailerThreadCount();
    int globalThreads = QThreadPool::globalInstance()->maxThreadCount();
    if(threads > globalThreads)
        threads = globalThreads;
    pool->setMaxThreadCount(threads);
}

void Thumbnailer::clearTasks() {
    pool->clear();
    pool->waitForDone();
}

std::shared_ptr<Thumbnail> Thumbnailer::getThumbnail(QString filePath, int size) {
    return ThumbnailerRunnable::generate(nullptr, filePath, size, false, false);
}

void Thumbnailer::generateThumbnails(QList<int> indexes, int size, bool crop, bool force) {
    pool->clear();
    for(int i = 0; i < indexes.count(); i++) {
        if(!dm->checkRange(indexes[i]))
            continue;
        QString filePath = dm->filePathAt(indexes[i]);
        if(!runningTasks.contains(filePath, size)) {
            startThumbnailerThread(filePath, size, crop, force);
        }
    }
}

void Thumbnailer::startThumbnailerThread(QString filePath, int size, bool crop, bool force) {
    auto runnable = new ThumbnailerRunnable(settings->useThumbnailCache() ? cache : nullptr, filePath, size, crop, force);
    connect(runnable, &ThumbnailerRunnable::taskStart, this, &Thumbnailer::onTaskStart);
    connect(runnable, &ThumbnailerRunnable::taskEnd, this, &Thumbnailer::onTaskEnd);
    runnable->setAutoDelete(true);
    pool->start(runnable);
}

void Thumbnailer::onTaskStart(QString filePath, int size) {
    runningTasks.insert(filePath, size);
}

void Thumbnailer::onTaskEnd(std::shared_ptr<Thumbnail> thumbnail, QString filePath) {
    runningTasks.remove(filePath, thumbnail->size());
    emit thumbnailReady(thumbnail, filePath);
}
