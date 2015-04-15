#include "image.h"
#include <time.h>


//use this one
Image::Image(QString _path) : QObject()
{
    path = _path;
    inUseFlag = false;
    image = NULL;
    movie = NULL;
    type = NONE;
    extension = NULL;
}

Image::~Image()
{
    delete image;
    delete movie;
}

void Image::detectType() {
    QFile file(path);
    file.open(QIODevice::ReadOnly);
    //read first 2 bytes to determine file format
    QByteArray startingBytes= file.read(2).toHex();
    file.close();

    if(startingBytes=="4749") {
        type=GIF;
        extension="GIF";
    }
    else if(startingBytes=="ffd8") {
        type=STATIC;
        extension="JPG";
    }
    else if(startingBytes=="8950") {
        type=STATIC;
        extension="PNG";
    }
    else if(startingBytes=="424d") {
        type=STATIC;
        extension="BMP";
    }
    else type = STATIC;
}

//load image data from disk
void Image::loadImage()
{
    mutex.lock();
    if(isLoaded()) {
        mutex.unlock();
        return;
    }
    detectType();
    if(type!=NONE)  {
        if(getType() == GIF) {
            movie = new QMovie();
            movie->setFormat("GIF");
            movie->setFileName(path);
            movie->jumpToFrame(0);
        }
        else if(getType() == STATIC) {
            image = new QImage();
            if(extension) {
                image = new QImage(path, extension);
            }
            else {
                image = new QImage(path); // qt will guess format
            }
        }
    }
    thumbnail();
    mutex.unlock();
}

void Image::unloadImage() {
    mutex.lock();
    if(isLoaded()) {
        delete movie;
        delete image;
        movie = NULL;
        image = NULL;
        qDebug() << "unloading " << this->path;
    }
    mutex.unlock();
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
        if(type == GIF) {
            return movie->currentImage().height();
        }
        if(type == STATIC) {
            return image->height();
        }
    }
    return 0;
}

int Image::width() {
    if(isLoaded()) {
        if(type == GIF) {
            return movie->currentImage().width();
        }
        if(type == STATIC) {
            return image->width();
        }
    }
    return 0;
}

QSize Image::size() {
    if(isLoaded()) {
        if(type == GIF) {
            return movie->currentImage().size();
        }
        if(type == STATIC) {
            return image->size();
        }
    }
    return QSize(0,0);
}

int Image::getType()
{
    return type;
}

QString Image::getPath()
{
    return path;
}

/*
bool Image::compare(Image* another) {
    if(getFileName() == another->getFileName() &&
       info.getLastModifiedDate() == another->getFileInfo().getLastModifiedDate() ) {
        return true;
    }
    return false;
}
*/

QImage* Image::rotated(int grad) {
    if(isLoaded()) {
        if(type==STATIC) {
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
        if(type==STATIC) {
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
