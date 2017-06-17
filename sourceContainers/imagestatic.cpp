#include "imagestatic.h"
#include <time.h>

//use this one
ImageStatic::ImageStatic(QString _path) {
    path = _path;
    loaded = false;
    image = NULL;
    imageInfo = new ImageInfo(path);
    unloadRequested = false;
}

ImageStatic::ImageStatic(ImageInfo *_info) {
    loaded = false;
    image = NULL;
    imageInfo = _info;
    path = imageInfo->filePath();
    unloadRequested = false;
}

ImageStatic::~ImageStatic() {
    delete image;
}

//load image data from disk
void ImageStatic::load() {
    lock();
    if(!imageInfo) {
        imageInfo = new ImageInfo(path);
    }
    if(isLoaded()) {
        unlock();
        return;
    }
    image = new QImage();
    image->load(path, imageInfo->extension());
    loaded = true;
    unlock();
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
