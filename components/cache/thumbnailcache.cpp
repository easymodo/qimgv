#include "thumbnailcache.h"

ThumbnailCache::ThumbnailCache() {
    cacheDirPath = settings->cacheDir();
}

QString ThumbnailCache::thumbnailPath(QString id) {
    return QString(cacheDirPath + id + ".jpg");
}

bool ThumbnailCache::exists(QString id) {
    QString filePath = thumbnailPath(id);
    QFileInfo file(filePath);
    if(file.exists() && file.isReadable()) {
        return true;
    } else {
        return false;
    }
}

void ThumbnailCache::saveThumbnail(QImage *image, QString id) {
    //mutex.lock();
    if(image) {
        QString filePath = thumbnailPath(id);
        //qDebug() << "saving thumnbail as: " << filePath;
        if(!image->save(filePath, "JPG", 96)) {
            qDebug() << "could not save thumbnail to " << filePath;
        }
    }
    //mutex.unlock();
}

QImage *ThumbnailCache::readThumbnail(QString id) {
    //mutex.lock();
    QString filePath = thumbnailPath(id);
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
            return nullptr;
        }
        //qDebug() << "reading thumbnail:" << filePath;
    } else {
        //qDebug() << "thumbnail does not exist:" << filePath;
        //mutex.unlock();
        return nullptr;
    }
}
