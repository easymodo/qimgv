#include "imagestatic.h"
#include <time.h>

//use this one
ImageStatic::ImageStatic(QString _path)
{
    path = _path;
    loaded = false;
    image = new QImage();
    type = STATIC;
    extension = NULL;
}

ImageStatic::~ImageStatic()
{
    delete image;
}

//load image data from disk
void ImageStatic::load()
{
    mutex.lock();
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
    generateThumbnail();
    mutex.unlock();
}

void ImageStatic::unload() {
    mutex.lock();
    if(isLoaded()) {
        delete image;
        image = new QImage();
        loaded = false;
    }
    mutex.unlock();
}

void ImageStatic::save(QString destinationPath) {
    if(isLoaded()) {
        image->save(destinationPath, extension, 100);
    }
}

void ImageStatic::save() {
    if(isLoaded()) {
        image->save(path, extension, 100);
    }
}

QPixmap* ImageStatic::generateThumbnail() {
    int size = globalSettings->s.value("thumbnailSize", 100).toInt();
    QPixmap *thumbnail = new QPixmap(size, size);
    QPixmap *tmp;
    if(!isLoaded()) {
        tmp = new QPixmap(path);
        *tmp = tmp->scaled(size*2, size*2, Qt::KeepAspectRatio, Qt::FastTransformation)
            .scaled(size, size, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    } else {
        tmp = new QPixmap();
        if(!image->isNull()) {
            *tmp = QPixmap::fromImage(image->scaled(size*2, size*2,
                                                          Qt::KeepAspectRatio,
                                                          Qt::FastTransformation)
                .scaled(size, size, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
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
    pix->convertFromImage(*image);
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

void ImageStatic::rotate(int grad) {
    mutex.lock();
    if(isLoaded()) {
        if(type==STATIC) {
            mutex.lock();
            QImage *img = rotated(grad);
            delete image;
            image = img;
            mutex.unlock();
        }
    }
    mutex.unlock();
}

void ImageStatic::crop(QRect newRect) {
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
