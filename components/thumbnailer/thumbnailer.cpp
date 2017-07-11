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
        QString filePath = dm->filePathAt(indexes[i]);
        if(!runningTasks.contains(filePath)) {
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
    connect(thumbnailerRunnable, SIGNAL(taskStart(QString)),
            this, SLOT(onTaskStart(QString)));
    connect(thumbnailerRunnable, SIGNAL(taskEnd(Thumbnail*, QString)),
            this, SLOT(onTaskEnd(Thumbnail*, QString)));
    thumbnailerRunnable->setAutoDelete(true);
    pool->start(thumbnailerRunnable);
}

void Thumbnailer::onTaskStart(QString path) {
    runningTasks.append(path);
}

void Thumbnailer::onTaskEnd(Thumbnail *thumbnail, QString path) {
    runningTasks.removeOne(path);
    emit thumbnailReady(thumbnail);
}
