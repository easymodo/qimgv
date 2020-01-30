#include "thumbnailer.h"

Thumbnailer::Thumbnailer(DirectoryManager *_dm) : dm(_dm) {
    thumbnailCache = new ThumbnailCache();
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

void Thumbnailer::generateThumbnails(QList<int> indexes, int size, bool cropSquare, bool forceGenerate) {
    pool->clear();
    for(int i = 0; i < indexes.count(); i++) {
        if(!dm->checkRange(indexes[i]))
            continue;
        QString filePath = dm->filePathAt(indexes[i]);
        if(!runningTasks.contains(filePath, size)) {
            startThumbnailerThread(filePath, size, cropSquare, forceGenerate);
        }
    }
}

void Thumbnailer::startThumbnailerThread(QString filePath, int size, bool cropSquare, bool forceGenerate) {
    auto runnable = new ThumbnailerRunnable(thumbnailCache, filePath, size, cropSquare, forceGenerate);
    connect(runnable, &ThumbnailerRunnable::taskStart, this, &Thumbnailer::onTaskStart);
    connect(runnable, &ThumbnailerRunnable::taskEnd, this, &Thumbnailer::onTaskEnd);
    runnable->setAutoDelete(true);
    pool->start(runnable);
}

void Thumbnailer::onTaskStart(QString path, int size) {
    runningTasks.insert(path, size);
}

void Thumbnailer::onTaskEnd(std::shared_ptr<Thumbnail> thumbnail, QString path) {
    runningTasks.remove(path, thumbnail->size());
    emit thumbnailReady(thumbnail);
}
