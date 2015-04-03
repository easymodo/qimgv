#include "image.h"
#include <time.h>


//use this one
Image::Image(FileInfo* _info) : QObject()
{
    image = new QImage();
    movie = new QMovie();
    info = _info;
    inUseFlag = false;
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
    QString path = info->getFilePath();
    if(info->getType()!=NONE)  {
        if(getType() == GIF) {
            movie->setFormat("GIF");
            movie->setFileName(info->getFilePath());
            movie->jumpToFrame(0);
            aspectRatio = (float)movie->currentImage().height()/
                    movie->currentImage().width();
            info->setHeight(movie->currentImage().height());
            info->setWidth(movie->currentImage().width());
        }
        else if(getType() == STATIC) {
            if(info->getExtension()) {
                image = new QImage(path, info->getExtension());
            }
            else {
                image = new QImage(path); // qt will guess format
            }
            aspectRatio = (float)image->height()/
                    image->width();
            info->setHeight(image->height());
            info->setWidth(image->width());
        }
        info->inUse = true;
    }
}

bool Image::useFlag() {
    return inUseFlag;
}

void Image::setUseFlag(bool flag) {
    inUseFlag=flag;
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

QImage* Image::getImage() const
{
    return image;
}

int Image::height() {
    if(info->getType() == GIF) {
        return movie->currentImage().height();
    }
    if(info->getType() == STATIC) {
        return image->height();
    }
    else return 1;
}

int Image::width() {
    if(info->getType() == GIF) {
        return movie->currentImage().width();
    }
    if(info->getType() == STATIC) {
        return image->width();
    }
    else return 1;
}

QSize Image::size() {
    if(info->getType() == GIF) {
        return movie->currentImage().size();
    }
    if(info->getType() == STATIC) {
        return image->size();
    }
    else return QSize(1,1);
}

float Image::getAspect() {
    return aspectRatio;
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
