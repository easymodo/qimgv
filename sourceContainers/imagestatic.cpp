#include "imagestatic.h"
#include <time.h>

//use this one
ImageStatic::ImageStatic(QString _path) {
    path = _path;
    loaded = false;
    image = NULL;
    type = STATIC;
    extension = NULL;
    info=NULL;
    sem = new QSemaphore(1);
    unloadRequested = false;
}

ImageStatic::ImageStatic(FileInfo *_info) {
    loaded = false;
    image = NULL;
    type = STATIC;
    extension = NULL;
    info=_info;
    path=info->getFilePath();
    sem = new QSemaphore(1);
    unloadRequested = false;
}

ImageStatic::~ImageStatic()
{
    delete image;
    delete sem;
}

//load image data from disk
void ImageStatic::load()
{
    int time= clock();
    lock();
    if(!info)
        info = new FileInfo(path);
    if(isLoaded()) {
        unlock();
        return;
    }
    guessType();
    if(extension) {
        image = new QImage(path, extension);
    }
    else {
        image = new QImage(path); // qt will guess format
    }
    loaded = true;
    unlock();
}

void ImageStatic::unload() {
    int z = sem->available();
    qDebug() << z << ": " << info->getFileName();
    if(z == 0) {
        unloadRequested = true;
        qDebug() << "set unload request " << info->getFileName();
    } else {
        qDebug() << "unloading immediately " << info->getFileName();
        unloadBlocking();
    }
}

void ImageStatic::unloadBlocking() {
    lock();
    if(isLoaded()) {
        delete image;
        image = NULL;
        loaded = false;
    }
    unlock();
}

void ImageStatic::save(QString destinationPath) {
    if(isLoaded()) {
        lock();
        image->save(destinationPath, extension, 100);
        unlock();
    }
}

void ImageStatic::lock() {
    qDebug() << "locking..";
    sem->acquire(1);
}

// performs unload if it was requested while object was blocked
void ImageStatic::unlock() {
    sem->release(1);
    if(unloadRequested) {
        qDebug() << "discarding via unblock " << info->getFileName();
        unloadRequested = false;
        unloadBlocking();
    }
}

void ImageStatic::save() {
    if(isLoaded()) {
        lock();
        image->save(path, extension, 100);
        unlock();
    }
}

QPixmap* ImageStatic::generateThumbnail() {
    int size = globalSettings->thumbnailSize();
    QPixmap *thumbnail = new QPixmap(size, size);
    QPixmap *tmp;
    if(!isLoaded()) {
        guessType();
        tmp = new QPixmap(path, extension);
        *tmp = tmp->scaled(size*2,
                           size*2,
                           Qt::KeepAspectRatioByExpanding,
                           Qt::FastTransformation)
                   .scaled(size,
                           size,
                           Qt::KeepAspectRatioByExpanding,
                           Qt::SmoothTransformation);
    } else {
        tmp = new QPixmap();
        if(!image->isNull()) {
            lock();
            *tmp = QPixmap::fromImage(
                        image->scaled(size*2,
                                      size*2,
                                      Qt::KeepAspectRatioByExpanding,
                                      Qt::FastTransformation)
                              .scaled(size,
                                      size,
                                      Qt::KeepAspectRatioByExpanding,
                                      Qt::SmoothTransformation));
            unlock();
        }
    }
    QRect target(0, 0, size,size);
    target.moveCenter(tmp->rect().center());
    *thumbnail = tmp->copy(target);
    delete tmp;
    return thumbnail;
}

QPixmap* ImageStatic::getPixmap()
{
    QPixmap *pix = new QPixmap();
    if(isLoaded()) {
        lock();
        pix->convertFromImage(*image);
        unlock();
    }
    return pix;
}

const QImage* ImageStatic::getImage() {
    const QImage* cPtr = image;
    return cPtr;
}

int ImageStatic::height() {
    if(isLoaded()) {
        return image->height();
    }
    return 0;
}

int ImageStatic::width() {
    if(isLoaded()) {
        return image->width();
    }
    return 0;
}

QSize ImageStatic::size() {
    if(isLoaded()) {
        return image->size();
    }
    return QSize(0,0);
}

QImage* ImageStatic::rotated(int grad) {
    if(isLoaded()) {
        lock();
        QImage *img = new QImage();
        QTransform transform;
        transform.rotate(grad);
        *img = image->transformed(transform, Qt::SmoothTransformation);
        unlock();
        return img;
    }
    else return NULL;
}

void ImageStatic::rotate(int grad) {
    if(isLoaded()) {
        QImage *img = rotated(grad);
        delete image;
        lock();
        image = img;
        unlock();
    }
}

void ImageStatic::crop(QRect newRect) {
    if(isLoaded()) {
        QImage *tmp = new QImage(newRect.size(), QImage::Format_ARGB32_Premultiplied);
        lock();
        *tmp = image->copy(newRect);
        delete image;
        image = tmp;
        unlock();
    }
}
