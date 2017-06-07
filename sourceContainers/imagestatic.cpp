#include "imagestatic.h"
#include <time.h>

//use this one
ImageStatic::ImageStatic(QString _path) {
    path = _path;
    loaded = false;
    image = NULL;
    fileInfo = new FileInfo(path, this);
    sem = new QSemaphore(1);
    unloadRequested = false;
}

ImageStatic::ImageStatic(FileInfo *_info) {
    loaded = false;
    image = NULL;
    fileInfo = _info;
    path = fileInfo->filePath();
    sem = new QSemaphore(1);
    unloadRequested = false;
}

ImageStatic::~ImageStatic() {
    delete image;
    delete fileInfo;
}

//load image data from disk
void ImageStatic::load() {
    QMutexLocker locker(&mutex);
    if(!fileInfo) {
        fileInfo = new FileInfo(path, this);
    }
    if(isLoaded()) {
        return;
    }
    image = new QImage(path, fileInfo->fileExtension());
    loaded = true;
}

void ImageStatic::save(QString destinationPath) {
    if(isLoaded()) {
        lock();
        image->save(destinationPath);
        unlock();
    }
}

void ImageStatic::save() {
    if(isLoaded()) {
        lock();
        image->save(path);
        unlock();
    }
}

QPixmap *ImageStatic::generateThumbnail(int size, bool squared) {
    Qt::AspectRatioMode method = squared?(Qt::KeepAspectRatioByExpanding):(Qt::KeepAspectRatio);
    QPixmap *tmp;
    if(!isLoaded()) {
        tmp = new QPixmap(path, fileInfo->fileExtension());
        *tmp = tmp->scaled(size * 2,
                           size * 2,
                           method,
                           Qt::FastTransformation)
               .scaled(size,
                       size,
                       method,
                       Qt::SmoothTransformation);
    } else {
        tmp = new QPixmap();
        if(!image->isNull()) {
            lock();
            *tmp = QPixmap::fromImage(
                       image->scaled(size * 2,
                                     size * 2,
                                     method,
                                     Qt::FastTransformation)
                       .scaled(size,
                               size,
                               method,
                               Qt::SmoothTransformation));
            unlock();
        }
    }
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

QPixmap *ImageStatic::getPixmap() {
    QPixmap *pix = new QPixmap();
    if(isLoaded()) {
        lock();
        pix->convertFromImage(*image);
        unlock();
    }
    return pix;
}

const QImage *ImageStatic::getImage() {
    return image;
}

int ImageStatic::height() {
    return isLoaded() ? image->height() : 0;
}

int ImageStatic::width() {
    return isLoaded() ? image->width() : 0;
}

QSize ImageStatic::size() {
    return isLoaded() ? image->size() : QSize(0, 0);
}

QImage *ImageStatic::rotated(int grad) {
    if(isLoaded()) {
        lock();
        QImage *img = new QImage();
        QTransform transform;
        transform.rotate(grad);
        *img = image->transformed(transform, Qt::SmoothTransformation);
        unlock();
        return img;
    }
    return NULL;
}

void ImageStatic::rotate(int grad) {
    if(isLoaded()) {
        QImage *img = rotated(grad);
        lock();
        delete image;
        image = img;
        unlock();
    }
}

void ImageStatic::crop(QRect newRect) {
    if(isLoaded()) {
        lock();
        QImage *tmp = new QImage(newRect.size(), image->format());
        *tmp = image->copy(newRect);
        delete image;
        image = tmp;
        unlock();
    }
}

QImage *ImageStatic::cropped(QRect newRect, QRect targetRes, bool upscaled) {
    if(isLoaded()) {
        QImage *cropped = new QImage(targetRes.size(), image->format());
        lock();
        if(upscaled) {
            QImage temp = image->copy(newRect);
            *cropped = temp.scaled(targetRes.size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
            QRect target(QPoint(0, 0), targetRes.size());
            target.moveCenter(cropped->rect().center());
            *cropped = cropped->copy(target);
        } else {
            newRect.moveCenter(image->rect().center());
            *cropped = image->copy(newRect);
        }
        unlock();
        return cropped;
    }
    return NULL;
}
