#include "imageanimated.h"
#include <time.h>

// TODO: this class is kinda useless now. redesign?

ImageAnimated::ImageAnimated(QString _path) {
    path = _path;
    loaded = false;
    mSize.setWidth(0);
    mSize.setHeight(0);
    fileInfo = new FileInfo(path, this);
}

ImageAnimated::ImageAnimated(FileInfo *_info) {
    loaded = false;
    fileInfo = _info;
    mSize.setWidth(0);
    mSize.setHeight(0);
    path = fileInfo->filePath();
}

ImageAnimated::~ImageAnimated() {
    if(fileInfo)
        delete fileInfo;
}

void ImageAnimated::load() {
    QMutexLocker locker(&mutex);
    if(isLoaded()) {
        return;
    }
    if(!fileInfo) {
        fileInfo = new FileInfo(path, this);
    }
    QPixmap pixmap(path, fileInfo->fileExtension());
    mSize = pixmap.size();
    loaded = true;
}

void ImageAnimated::save(QString destinationPath) {
    QFile file(path);
    if(file.exists()) {
        if (!file.copy(destinationPath)) {
            qDebug() << "Unable to save file.";
        }
    } else {
        qDebug() << "Unable to save file. Perhaps the source file was deleted?";
    }
}

void ImageAnimated::save() {
    //TODO
}

QPixmap *ImageAnimated::generateThumbnail(bool squared) {
    Qt::AspectRatioMode method = squared?(Qt::KeepAspectRatioByExpanding):(Qt::KeepAspectRatio);
    int size = settings->thumbnailSize();
    QPixmap *tmp;
    tmp = new QPixmap(path, fileInfo->fileExtension());
    *tmp = tmp->scaled(size * 2,
                       size * 2,
                       method,
                       Qt::FastTransformation)
               .scaled(size,
                       size,
                       method,
                       Qt::SmoothTransformation);
    if(squared) {
        QRect target(0, 0, size, size);
        target.moveCenter(tmp->rect().center());
        QPixmap *thumbnail = new QPixmap(size, size);
        *thumbnail = tmp->copy(target);
        delete tmp;
        return thumbnail;
    } else {
        return tmp;
    }
}

// in case of gif returns current frame
QPixmap *ImageAnimated::getPixmap() {
    QPixmap *pix = new QPixmap(path, fileInfo->fileExtension());
    return pix;
}

const QImage *ImageAnimated::getImage() {
    QImage *img = new QImage();
    const QImage *cPtr = img;
    return cPtr;
}

QMovie *ImageAnimated::getMovie() {
    QMovie *_movie = new QMovie();
    _movie->setFileName(path);
    _movie->setFormat(fileInfo->fileExtension());
    _movie->jumpToFrame(0);
    return _movie;
}

int ImageAnimated::height() {
    return mSize.height();
}

int ImageAnimated::width() {
    return mSize.width();
}

QSize ImageAnimated::size() {
    return mSize;
}

void ImageAnimated::rotate(int grad) {
}

void ImageAnimated::crop(QRect newRect) {
}
