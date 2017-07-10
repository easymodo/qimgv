#include "imageanimated.h"
#include <time.h>

// TODO: this class is kinda useless now. redesign?

ImageAnimated::ImageAnimated(QString _path) {
    path = _path;
    loaded = false;
    mSize.setWidth(0);
    mSize.setHeight(0);
    imageInfo = new ImageInfo(path);
}

ImageAnimated::ImageAnimated(ImageInfo *_info) {
    loaded = false;
    imageInfo = _info;
    mSize.setWidth(0);
    mSize.setHeight(0);
    path = imageInfo->filePath();
}

ImageAnimated::~ImageAnimated() {
}

void ImageAnimated::load() {
    if(isLoaded()) {
        return;
    }
    if(!imageInfo) {
        imageInfo = new ImageInfo(path);
    }
    QPixmap pixmap(path, imageInfo->extension());
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

// in case of gif returns current frame
QPixmap *ImageAnimated::getPixmap() {
    QPixmap *pix = new QPixmap(path, imageInfo->extension());
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
    _movie->setFormat(imageInfo->extension());
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
    Q_UNUSED(grad)
}

void ImageAnimated::crop(QRect newRect) {
    Q_UNUSED(newRect)
}
