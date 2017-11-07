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
    if(!imageInfo) {
        imageInfo = new ImageInfo(path);
    }
    if(isLoaded()) {
        return;
    }
    image = new QImage();
    image->load(path, imageInfo->extension());
    QImage::Format format = image->format();
    int depth = image->depth();
    // Convert to Grayscale8 / RGB32 / ARGB32
    // Allows fast conversion to FreeImage formats later on
    if(format == QImage::Format_Indexed8) {
        QImage *image2 = new QImage();
        *image2 = image->convertToFormat(QImage::Format_RGB32);
        delete image;
        image = image2;
    } else if(format != QImage::Format_RGB32 && format != QImage::Format_ARGB32 && format != QImage::Format_Grayscale8) {
        QImage *image2 = new QImage();
        switch (depth) {
            case 1:
            case 8:
                *image2 = image->convertToFormat(QImage::Format_Grayscale8);
                delete image;
                image = image2;
                break;
            case 24:
                *image2 = image->convertToFormat(QImage::Format_RGB32);
                delete image;
                image = image2;
                break;
            case 32:
                *image2 = image->convertToFormat(QImage::Format_ARGB32);
                delete image;
                image = image2;
                break;
            default: // shouldn't happen
                *image2 = image->convertToFormat(QImage::Format_ARGB32);
                delete image;
                image = image2;
                break;
        }
    }
    loaded = true;
}

void ImageStatic::save(QString destinationPath) {
    if(isLoaded()) {
        image->save(destinationPath);
    }
}

void ImageStatic::save() {
    if(isLoaded()) {
        image->save(path);
    }
}

QPixmap *ImageStatic::getPixmap() {
    QPixmap *pix = new QPixmap();
    if(isLoaded()) {
        pix->convertFromImage(*image);
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
        QImage *img = new QImage();
        QTransform transform;
        transform.rotate(grad);
        *img = image->transformed(transform, Qt::SmoothTransformation);
        return img;
    }
    return NULL;
}

void ImageStatic::rotate(int grad) {
    if(isLoaded()) {
        QImage *img = rotated(grad);
        delete image;
        image = img;
    }
}

void ImageStatic::crop(QRect newRect) {
    if(isLoaded()) {
        QImage *tmp = new QImage(newRect.size(), image->format());
        *tmp = image->copy(newRect);
        delete image;
        image = tmp;
    }
}

QImage *ImageStatic::cropped(QRect newRect, QRect targetRes, bool upscaled) {
    if(isLoaded()) {
        QImage *cropped = new QImage(targetRes.size(), image->format());
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
        return cropped;
    }
    return NULL;
}
