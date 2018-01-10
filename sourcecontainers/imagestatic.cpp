#include "imagestatic.h"
#include <time.h>

ImageStatic::ImageStatic(QString _path) {
    path = _path;
    loaded = false;
    image = NULL;
    imageEdited = NULL;
    imageInfo = new ImageInfo(path);
    load();
}

ImageStatic::~ImageStatic() {
    if(image)
        delete image;
    if(imageEdited)
        delete imageEdited;
}

//load image data from disk
void ImageStatic::load() {
    if(isLoaded()) {
        return;
    }
    if(!imageInfo) {
        imageInfo = new ImageInfo(path);
    }
    image = new QImage();
    image->load(path, imageInfo->extension());
    loaded = true;
}

bool ImageStatic::save(QString destPath) {
    return isEdited()?imageEdited->save(destPath):image->save(destPath);
}

bool ImageStatic::save() {
    return isEdited()?imageEdited->save(path):image->save(path);
}

QPixmap *ImageStatic::getPixmap() {
    QPixmap *pix = new QPixmap();
    isEdited()?pix->convertFromImage(*imageEdited):pix->convertFromImage(*image);
    return pix;
}

const QImage *ImageStatic::getSourceImage() {
    return image;
}

const QImage *ImageStatic::getImage() {
    return isEdited()?imageEdited:image;
}

int ImageStatic::height() {
    return isEdited()?imageEdited->height():image->height();
}

int ImageStatic::width() {
    return isEdited()?imageEdited->width():image->width();
}

QSize ImageStatic::size() {
    return isEdited()?imageEdited->size():image->size();
}

bool ImageStatic::setEditedImage(QImage *imageEditedNew) {
    if(imageEditedNew && imageEditedNew->width() != 0) {
        discardEditedImage();
        imageEdited = imageEditedNew;
        edited = true;
        return true;
    } else {
        return false;
    }
}

bool ImageStatic::discardEditedImage() {
    if(imageEdited) {
        delete imageEdited;
        imageEdited = NULL;
        edited = false;
        return true;
    }
    return false;
}


