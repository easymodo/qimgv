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
}

//load image data from disk
void Image::loadImage()
{
    QString path = info->getFilePath();
    if(info->getType()!=NONE)  {
        if(getType() == GIF) {
            movie->setFileName(info->getFilePath());
            movie->jumpToFrame(0);
            aspectRatio = (float)movie->currentImage().height()/
                    movie->currentImage().width();
            info->setHeight(movie->currentImage().height());
            info->setWidth(movie->currentImage().width());
        }
        else if(getType() == STATIC) {
       //     QImage *tmp = new QImage(path); // possibly created in worker thread
            image = new QImage(path);//tmp->convertToFormat(QImage::Format_ARGB32_Premultiplied);
            //delete tmp;
            aspectRatio = (float)image->height()/
                    image->width();
            info->setHeight(image->height());
            info->setWidth(image->width());
            qDebug() << "FORMAT: " << image->format();
        }
        info->inUse = true;
    }
}

bool Image::isInUse() {
    return inUseFlag;
}

void Image::setInUse(bool arg) {
    inUseFlag=arg;
}

int Image::ramSize() {
    if(getType() == GIF) {
        return 1;

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

QImage* Image::getImage()
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

qint64 Image::getSize() {
    return info->getSize();
}

QString Image::getName() {
    return info->getName();
}

FileInfo* Image::getInfo() const {
    return info;
}

bool Image::compare(Image* another) {
    if(getName() == another->getName() &&
       info->getLastModified() == another->getInfo()->getLastModified() ) {
        return true;
    }
    return false;
}
