#include "thumbnailcache.h"

ThumbnailCache::ThumbnailCache() {
    cacheDirPath = settings->cacheDir();
}

QString ThumbnailCache::thumbnailPath(QString hash) {
    return QString(cacheDirPath + hash + ".jpg");
}

bool ThumbnailCache::exists(QString hash) {
    QString filePath = thumbnailPath(hash);
    QFileInfo file(filePath);
    if(file.exists() && file.isReadable()) {
        return true;
    } else {
        return false;
    }
}

void ThumbnailCache::saveThumbnail(QImage *image, QString hash) {
    //mutex.lock();
    if(image) {
        QString filePath = thumbnailPath(hash);
        //qDebug() << "saving thumnbail as: " << filePath;
        if(!image->save(filePath, "JPG", 96)) {
            qDebug() << "could not save thumbnail to " << filePath;
        }
    }
    //mutex.unlock();
}

QImage *ThumbnailCache::readThumbnail(QString hash) {
    //mutex.lock();
    QString filePath = thumbnailPath(hash);
    QFileInfo file(filePath);
    if(file.exists() && file.isReadable()) {
        QImage *thumb = new QImage();
        if(thumb->load(filePath)) {
            //qDebug() << filePath << "succesfully loaded";
            //mutex.unlock();
            return thumb;
        } else {
            delete thumb;
            //qDebug() << "file exists but does not appear to be a valid image(expected jpg file): " << filePath;
            //mutex.unlock();
            return NULL;
        }
        //qDebug() << "reading thumbnail:" << filePath;
    } else {
        //qDebug() << "thumbnail does not exist:" << filePath;
        //mutex.unlock();
        return NULL;
    }
}
