#include "thumbnailer.h"

Thumbnailer::Thumbnailer(DirectoryManager *_dm) : dm(_dm) {
    thumbnailCache = new ThumbnailCache();
}

void Thumbnailer::generateThumbnailFor(QList<int> indexes, int size) {
    for(int i = 0; i < indexes.count(); i++) {
        startThumbnailerThread(indexes[i], size);
    }
}

void Thumbnailer::startThumbnailerThread(int index, int size) {
    ThumbnailerRunnable *thumbnailerRunnable = new ThumbnailerRunnable(
                                                thumbnailCache,
                                                dm->filePathAt(index),
                                                size,
                                                settings->squareThumbnails());
    connect(thumbnailerRunnable, SIGNAL(thumbnailReady(Thumbnail*)),
            this, SIGNAL(thumbnailReady(Thumbnail*)));
    thumbnailerRunnable->setAutoDelete(true);
    QThreadPool::globalInstance()->start(thumbnailerRunnable);
}
