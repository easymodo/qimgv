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

void Thumbnailer::generateThumbnailFor(QList<int> indexes, int size) {
    pool->clear();
    for(int i = 0; i < indexes.count(); i++) {
        if(!dm->checkRange(indexes[i]))
            continue;
        QString filePath = dm->filePathAt(indexes[i]);
        if(!runningTasks.contains(filePath,  size)) {
            startThumbnailerThread(filePath, size);
        }
    }
}

void Thumbnailer::startThumbnailerThread(QString filePath, int size) {
    ThumbnailerRunnable *thumbnailerRunnable = new ThumbnailerRunnable(
                                                thumbnailCache,
                                                filePath,
                                                size,
                                                settings->squareThumbnails());
    connect(thumbnailerRunnable, SIGNAL(taskStart(QString, int)),
            this, SLOT(onTaskStart(QString, int)));
    connect(thumbnailerRunnable, SIGNAL(taskEnd(std::shared_ptr<Thumbnail>, QString)),
            this, SLOT(onTaskEnd(std::shared_ptr<Thumbnail>, QString)));
    thumbnailerRunnable->setAutoDelete(true);
    pool->start(thumbnailerRunnable);
}

void Thumbnailer::onTaskStart(QString path, int size) {
    runningTasks.insert(path, size);
}

void Thumbnailer::onTaskEnd(std::shared_ptr<Thumbnail> thumbnail, QString path) {
    runningTasks.remove(path, thumbnail->size());
    emit thumbnailReady(thumbnail);
}
