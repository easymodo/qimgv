#include "image.h"
#include <time.h>


//use this one
Image::Image(FileInfo* _info) : QObject()
{
    info = _info;
    inUseFlag = false;
    image = NULL;
    movie = NULL;
}

Image::~Image()
{
    delete image;
    delete movie;
    delete info;
}

//load image data from disk
void Image::loadImage()
{
    mutex.lock();
    if(isLoaded()) {
        mutex.unlock();
        qDebug() << "IMAGE already loaded";
        return;
    }
    QString path = info->getFilePath();
    if(info->getType()!=NONE)  {
        if(getType() == GIF) {
            movie = new QMovie();
            movie->setFormat("GIF");
            movie->setFileName(info->getFilePath());
            movie->jumpToFrame(0);
        }
        else if(getType() == STATIC) {
            image = new QImage();
            if(info->getExtension()) {
                image = new QImage(path, info->getExtension());
            }
            else {
                image = new QImage(path); // qt will guess format
            }
        }
        info->inUse = true;
    }
    thumbnail();
    mutex.unlock();
}

void Image::unloadImage() {
    mutex.lock();
    if(isLoaded()) {
        delete movie;
        delete image;
    }
    mutex.unlock();
}

bool Image::useFlag() {
    return inUseFlag;
}

void Image::setUseFlag(bool flag) {
    inUseFlag=flag;
}

QImage Image::thumbnail() {
    int size = globalSettings->s.value("thumbnailSize", 100).toInt();
    QImage thumbnail;
    bool unloadFlag = false;
    if(!isLoaded()) {
        loadImage();
        unloadFlag = true;
    }
    if(getType() == GIF) {
        thumbnail = movie->currentImage()
                .scaled(size*2, size*2, Qt::KeepAspectRatio)
                .scaled(size, size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    } else if(getType() == STATIC) {
        thumbnail = image->scaled(size*2, size*2, Qt::KeepAspectRatio)
                .scaled(size, size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    if(unloadFlag) {
        unloadImage();
    }
    return thumbnail;
}

bool Image::isLoaded() {
    if(!image && !movie) return false;
    else return true;
}

int Image::ramSize() {
    if(getType() == GIF) { // maybe wrong
        return movie->frameCount()*movie->currentImage().byteCount()/(1024*1024);
    }
    else if(getType() == STATIC) {
        return image->byteCount()/(1024*1024);
    }
    else return 0;
}

QMovie* Image::getMovie()
{
    return movie;
}

const QImage* Image::getImage()
{
    return const_cast<const QImage*>(image);
}

int Image::height() {
    if(isLoaded()) {
        if(info->getType() == GIF) {
            return movie->currentImage().height();
        }
        if(info->getType() == STATIC) {
            return image->height();
        }
    }
    return 0;
}

int Image::width() {
    if(isLoaded()) {
        if(info->getType() == GIF) {
            return movie->currentImage().width();
        }
        if(info->getType() == STATIC) {
            return image->width();
        }
    }
    return 0;
}

QSize Image::size() {
    if(isLoaded()) {
        if(info->getType() == GIF) {
            return movie->currentImage().size();
        }
        if(info->getType() == STATIC) {
            return image->size();
        }
    }
    return QSize(0,0);
}

int Image::getType()
{
    return info->getType();
}

QString Image::getPath()
{
    return info->getFilePath();
}

qint64 Image::getFileSize() {
    return info->getFileSize();
}

QString Image::getFileName() {
    return info->getFileName();
}

FileInfo* Image::getFileInfo() const {
    return info;
}

bool Image::compare(Image* another) {
    if(getFileName() == another->getFileName() &&
       info->getLastModifiedDate() == another->getFileInfo()->getLastModifiedDate() ) {
        return true;
    }
    return false;
}

QImage* Image::rotated(int grad) {
    if(isLoaded()) {
        if(info->getType()==STATIC) {
            QImage *img = new QImage();
            QTransform transform;
            transform.rotate(grad);
            *img = image->transformed(transform, Qt::SmoothTransformation);
            return img;
        }
    }
    else return NULL;
}

void Image::rotate(int grad) {
    if(isLoaded()) {
        if(info->getType()==STATIC) {
            mutex.lock();
            QImage *img = rotated(grad);
            delete image;
            image = img;
            mutex.unlock();
        }
    }
}

void Image::crop(QRect newRect) {
    mutex.lock();
    if(isLoaded()) {
        if(getType() == STATIC) {
            QImage *tmp = new QImage(newRect.size(), QImage::Format_ARGB32_Premultiplied);
            *tmp = image->copy(newRect);
            delete image;
            image = tmp;
        }
        if(getType() == GIF) {
            // someday later
        }
    }
    mutex.unlock();
}
