#include "imagestatic.h"
#include <time.h>

//use this one
ImageStatic::ImageStatic(QString _path)
{
    path = _path;
    loaded = false;
    image = NULL;
    type = STATIC;
    extension = NULL;
    info=NULL;
}

ImageStatic::~ImageStatic()
{
    delete image;
}

//load image data from disk
void ImageStatic::load()
{
    mutex.lock();
    info = new FileInfo(path);
    if(isLoaded()) {
        mutex.unlock();
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
    mutex.unlock();
}

void ImageStatic::unload() {
    mutex.lock();
    if(isLoaded()) {
        delete image;
        image = NULL;
        loaded = false;
    }
    mutex.unlock();
}

void ImageStatic::save(QString destinationPath) {
    if(isLoaded()) {
        mutex.lock();
        image->save(destinationPath, extension, 100);
        mutex.unlock();
    }
}

void ImageStatic::save() {
    if(isLoaded()) {
        mutex.lock();
        image->save(path, extension, 100);
        mutex.unlock();
    }
}

QPixmap* ImageStatic::generateThumbnail() {
    int size = globalSettings->s.value("thumbnailSize", 100).toInt();
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
            mutex.lock();
            *tmp = QPixmap::fromImage(
                        image->scaled(size*2,
                                      size*2,
                                      Qt::KeepAspectRatioByExpanding,
                                      Qt::FastTransformation)
                              .scaled(size,
                                      size,
                                      Qt::KeepAspectRatioByExpanding,
                                      Qt::SmoothTransformation));
            mutex.unlock();
        }
    }
    QRect target(0, 0, size,size);
    target.moveCenter(tmp->rect().center());
    *thumbnail = tmp->copy(target);
    return thumbnail;
}

// in case of gif returns current frame
QPixmap* ImageStatic::getPixmap()
{
    QPixmap *pix = new QPixmap();
    if(isLoaded()) {
        mutex.lock();
        pix->convertFromImage(*image);
        mutex.unlock();
    }
    return pix;
}

// todo: const
QImage* ImageStatic::getImage() {
    return image;
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
        mutex.lock();
        QImage *img = new QImage();
        QTransform transform;
        transform.rotate(grad);
        *img = image->transformed(transform, Qt::SmoothTransformation);
        mutex.unlock();
        return img;
    }
    else return NULL;
}

void ImageStatic::rotate(int grad) {
    if(isLoaded()) {
        QImage *img = rotated(grad);
        delete image;
        mutex.lock();
        image = img;
        mutex.unlock();
    }
}

void ImageStatic::crop(QRect newRect) {
    if(isLoaded()) {
        QImage *tmp = new QImage(newRect.size(), QImage::Format_ARGB32_Premultiplied);
        mutex.lock();
        *tmp = image->copy(newRect);
        delete image;
        image = tmp;
        mutex.unlock();
    }
}
