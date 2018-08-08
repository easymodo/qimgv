#include "imageanimated.h"
#include <time.h>

// TODO: this class is kinda useless now. redesign?

ImageAnimated::ImageAnimated(QString _path)
    : Image(_path)
{
    mSize.setWidth(0);
    mSize.setHeight(0);
    load();
}

ImageAnimated::ImageAnimated(std::unique_ptr<DocumentInfo> _info)
    : Image(std::move(_info))
{
    mSize.setWidth(0);
    mSize.setHeight(0);
    load();
}

ImageAnimated::~ImageAnimated() {
}

void ImageAnimated::load() {
    if(isLoaded()) {
        return;
    }
    QPixmap pixmap(mPath, mDocInfo->extension());
    mSize = pixmap.size();
    mLoaded = true;
}

bool ImageAnimated::save(QString destPath) {
    QFile file(mPath);
    if(file.exists()) {
        if(!file.copy(destPath)) {
            qDebug() << "Unable to save file.";
            return false;
        } else {
            return true;
        }
    } else {
        qDebug() << "Unable to save file. Perhaps the source file was deleted?";
        return false;
    }
}

bool ImageAnimated::save() {
    //TODO
    return false;
}

// in case of gif returns current frame
std::unique_ptr<QPixmap> ImageAnimated::getPixmap() {
    return std::unique_ptr<QPixmap>(new QPixmap(mPath, mDocInfo->extension()));
}

// wait what?
std::shared_ptr<const QImage> ImageAnimated::getImage() {
    std::shared_ptr<const QImage> img(new QImage());
    return img;
}

std::unique_ptr<QMovie> ImageAnimated::getMovie() {
    std::unique_ptr<QMovie> movie(new QMovie());
    movie->setFileName(mPath);
    movie->setFormat(mDocInfo->extension());
    movie->jumpToFrame(0);
    return movie;
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
